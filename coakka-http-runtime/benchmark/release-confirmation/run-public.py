from pathlib import Path
import os,sys,json,hashlib,subprocess,time,signal,urllib.request,re,gzip,shutil
here=Path.cwd()
out=here/'attempts'/sys.argv[2]
out.mkdir(exist_ok=False)
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()
node=Path(os.environ['BENCHMARK_NODE']).resolve(strict=True)
bun=Path(os.environ['BENCHMARK_BUN']).resolve(strict=True)
java=Path(os.environ['BENCHMARK_JAVA']).resolve(strict=True)
std=here/'kotlin-stdlib-2.3.10.jar'
jar=here/'coakka-http-runtime-jvm-1.1.2.jar'
jvm=here/'jvm/native'
sdk=here/'sdk'
py=here/'python/bin/python'
py_root=Path(subprocess.check_output(
    [str(py), '-I', '-c', 'import coakka_http; print(coakka_http.__path__[0])'],
    text=True).strip()).resolve(strict=True)
assert py_root.is_relative_to(here/'python')
commands={k:[str(v),str(here/k/'server.mjs')] for k,v in [('node',node),('bun',bun)]}
commands.update(python=[str(py),'-I','-u',str(here/'server.py')],go=[str(here/'go-fixed'),'coakka','18430'],
 jvm=[str(java),'-XX:ActiveProcessorCount=2','-Dcoakka.http.host.path='+str(jvm/'libcoakka_http_host.so'),'-Dcoakka.http.bridge.path='+str(jvm/'libcoakka_http_jvm.so'),'-cp',str(here/'classes')+':'+str(jar)+':'+str(std),'FixedServer'])
paths={
 'python':[here/'server.py',*py_root.glob('*.py'),*py_root.glob('native/*.so*')],
 'go':[here/'go-fixed',sdk/'lib/libcoakka_http_host.so.1.1.2'],
 'jvm':[jar,here/'classes/FixedServer.class',jvm/'libcoakka_http_host.so',jvm/'libcoakka_http_jvm.so'],
}
for key in ('node','bun'):
 package=here/key/'node_modules'/('coakka-http-runtime-'+key)
 assert json.loads((package/'package.json').read_text())['version']=='1.1.2'
 platform=package/'platforms/linux-arm64'
 paths[key]=[here/key/'server.mjs',package/'src/index.mjs',platform/'src/index.mjs',*platform.glob('native/*')]
expected={key:{str(p.resolve()):sha(p) for p in values} for key,values in paths.items()}
record={'complete':False,'commands':commands,'expected_loaded':expected,'host':subprocess.check_output(['uname','-a'],text=True).strip(),'h2load_sha256':sha('/usr/bin/h2load'),'engine_sha256':{k:sha(v[0])for k,v in commands.items()},'runs':[]}
(out/'inputs.json').write_text(json.dumps(record,indent=2)+'\n')
governors=sorted(Path('/sys/devices/system/cpu').glob('cpu[0-3]/cpufreq/scaling_governor'));old=[p.read_text().strip()for p in governors];(out/'governors-before.json').write_text(json.dumps(old));server=None
base_env=os.environ.copy()
for k in ['COAKKA_HTTP_JAVASCRIPT_ADDON','LD_PRELOAD','LD_LIBRARY_PATH','NODE_OPTIONS','BUN_OPTIONS','JAVA_TOOL_OPTIONS','JDK_JAVA_OPTIONS','COAKKA_CPU_PROFILE']:base_env.pop(k,None)
base_env.update(COAKKA_BENCH_CPU_POLICY='auto',GOMAXPROCS='2')
def write_gov(values):
 for p,v in zip(governors,values):subprocess.run(['sudo','tee',str(p)],input=v,text=True,stdout=subprocess.DEVNULL,check=True)
def thermal(log,phase):
 t=int(Path('/sys/class/thermal/thermal_zone0/temp').read_text());th=subprocess.check_output(['vcgencmd','get_throttled'],text=True).strip();log.write(json.dumps(dict(phase=phase,time=time.time(),temperature_mC=t,throttle=th))+'\n');log.flush();return t,th

def stop():
 global server
 if server is None:return
 server.send_signal(signal.SIGTERM)
 try:code=server.wait(timeout=35)
 except subprocess.TimeoutExpired:
  server.kill();server.wait();raise RuntimeError('owned fixture shutdown timed out')
 server=None
 assert code in [0,143,-15],code

def interrupt(sig,frame):raise KeyboardInterrupt()
signal.signal(signal.SIGTERM,interrupt)
try:
 # Three fresh processes per engine and workload; reverse engine order on round2.
 for round in range(1,4):
  for engine in (['python','jvm','node','bun','go']if round!=2 else ['jvm','go','python','bun','node']):
   for mode in (['fresh','shared']if round!=2 else ['shared','fresh']):
    lane=f'{round}-{engine}-{mode}'
    if lane!=sys.argv[1]:continue
    d=out/lane;d.mkdir();result={'lane':lane,'engine':engine,'mode':mode,'round':round,'complete':False};record['runs'].append(result)
    with (d/'thermal.jsonl').open('w')as therm:
     write_gov(old)
     good=0
     for attempt in range(28):
      t,th=thermal(therm,'admission');good=good+1 if t<=44000 and th=='throttled=0x0' else 0
      if good==3:break
      time.sleep(2 if good else 5)
     else:raise RuntimeError('thermal admission timeout')
     write_gov(['performance']*4)
     with (d/'server.log').open('w')as slog:
      env={**base_env,'RESPONSE_MODE':mode};
      server=subprocess.Popen(['taskset','-c','0-1',*commands[engine]],stdout=slog,stderr=subprocess.STDOUT,env=env)
      for attempt in range(200):
       assert server.poll()is None,'fixture exited before ready'
       try:
        with urllib.request.urlopen('http://127.0.0.1:18430/fixed',timeout=1)as reply:assert reply.status==200 and reply.read()==b'0123456789abcdef0123456789abcdef'
        if 'coakka-runtime-info='in(d/'server.log').read_text():break
       except (OSError,AssertionError):pass
       time.sleep(.1)
      else:raise RuntimeError('fixture readiness timeout')
      text=(d/'server.log').read_text();actual={}
      for l in text.splitlines():
       if l.startswith(('identity-image=','identity-module=','identity-fixture=')):
        p,h=l.split('=',1)[1].split('\t');actual[p]=h
      assert actual==expected[engine],(engine,actual,expected[engine])
      if engine=='go':assert 'identity-module-version=v1.1.2' in text
      maps=Path(f'/proc/{server.pid}/maps').read_text();(d/'loaded-maps.txt').write_text(maps)
      native={l[l.index('/'):].strip()for l in maps.splitlines()if '/'in l and 'coakka_http'in l};assert native=={p for p in expected[engine]if 'coakka_http'in Path(p).name},native
      info=json.loads(next(l.split('=',1)[1]for l in text.splitlines()if l.startswith('coakka-runtime-info=')));result['runtime_info']=info;result['identity']=actual
      def normalized(value):
       return {re.sub(r'[^a-z]','',k.lower()):normalized(v) for k,v in value.items()} if isinstance(value,dict) else value
      def fields(value,key):
       return [v for k,v in value.items() if k==key]+[x for v in value.values() if isinstance(v,dict) for x in fields(v,key)]
      info_norm=normalized(info)
      assert 64 in fields(info_norm,'requestnotificationbatchsize'),info
      assert 32 in fields(info_norm,'terminalnotificationbatchsize'),info
      assert 2 in fields(info_norm,'selectedcpucount'),info
      assert 1 in fields(info_norm,'activeeventloops'),info
      with(d/'warmup.log').open('w')as log:subprocess.run(['taskset','-c','2-3','h2load','--h1','-c','64','-t','2','-D',('30s'if engine=='jvm'else'5s'),'http://127.0.0.1:18430/fixed'],stdout=log,stderr=subprocess.STDOUT,check=True,timeout=45)
      t,th=thermal(therm,'post-warmup');assert t<50000 and th=='throttled=0x0','warmup thermal rejection'
      result['timed_started']=time.time()
      command=['taskset','-c','2-3','h2load','--h1','-c','64','-t','2','-D','10s','--log-file='+str(d/'requests.tsv'),'http://127.0.0.1:18430/fixed']
      with (d/'h2load.log').open('w') as log, subprocess.Popen(command,stdout=log,stderr=subprocess.STDOUT) as client:
       try:
        while client.poll() is None:
         t,th=thermal(therm,'timed')
         assert th=='throttled=0x0','timed throttling'
         time.sleep(0.5)
        assert client.returncode==0,client.returncode
       finally:
        if client.poll() is None:client.terminate();client.wait(timeout=5)
      result['timed_finished']=time.time();(d/'process-status.txt').write_text(Path(f'/proc/{server.pid}/status').read_text());stop()
     text=(d/'h2load.log').read_text();assert '0 failed, 0 errored, 0 timeout'in text;assert 'benchmark-shutdown=pass'in(d/'server.log').read_text()
     t,th=thermal(therm,'post-load');assert th=='throttled=0x0','post-load throttling'
    with(d/'requests.tsv').open('rb')as src,gzip.open(d/'requests.tsv.gz','wb')as dst:shutil.copyfileobj(src,dst)
    (d/'requests.tsv').unlink();result['rate']=float(re.search(r'finished in .*?, ([\d,.]+) req/s',text).group(1).replace(',',''));result['complete']=True
    (out/'state.json').write_text(json.dumps(record,indent=2)+'\n');print(lane,result['rate'],flush=True)
 record['complete']=True
finally:
 try:stop()
 finally:
  write_gov(old);after=[p.read_text().strip()for p in governors];(out/'governors-after.json').write_text(json.dumps(after));record['governors_restored']=old==after;(out/'state.json').write_text(json.dumps(record,indent=2)+'\n')
