import gzip, hashlib, json, math, os, re, signal, statistics, subprocess, time, urllib.request
from pathlib import Path
root=Path.cwd()
out=root/'full-retest-runs'
out.mkdir()
lib=(root/'sdk/lib/libcoakka_http_host.so.1.1.2').resolve()
fixture=(root/'fixture').resolve()
sha=lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
assert sha(lib)=='25686f3805f5344631d6d66dff351df3cdac577a84d558f8ab7e86f1f42ef7c8'
url='http://127.0.0.1:18431/fixed'
gov=sorted(Path('/sys/devices/system/cpu').glob('cpu[0-3]/cpufreq/scaling_governor'))
old=[p.read_text().strip() for p in gov]
def governors(values):
    for p,v in zip(gov,values): subprocess.run(['sudo','-n','tee',p],input=v,text=True,stdout=subprocess.DEVNULL,check=True)
def thermal(phase):
    t=int(Path('/sys/class/thermal/thermal_zone0/temp').read_text())/1000
    th=subprocess.check_output(['vcgencmd','get_throttled'],text=True).strip()
    trace.append(dict(phase=phase,c=t,throttle=th))
    return th=='throttled=0x0'
def load(name,seconds,args=()):
    with (d/(name+'.log')).open('w') as f:
        proc=subprocess.Popen(['taskset','-c','2-3','h2load','--h1','-c','64','-t','2','-D',f'{seconds}s',*args,url],stdout=f,stderr=subprocess.STDOUT)
        try:
            deadline=time.monotonic()+seconds+10
            while proc.poll() is None:
                assert thermal(name) and time.monotonic()<deadline
                time.sleep(.5)
            assert proc.returncode==0 and thermal(name+'-end')
        finally:
            if proc.poll() is None: proc.kill(); proc.wait()
rows=[]
try:
    for attempt in range(1,7):
        if len(rows)==3: break
        d=out/str(attempt); d.mkdir(); trace=[]
        try:
            good=0
            for _ in range(150):
                good=good+1 if thermal('admission') and trace[-1]['c']<=44 else 0
                if good==3: break
                time.sleep(2)
            else: raise RuntimeError('cooldown timeout')
            governors(['performance']*len(gov))
            with (d/'server.log').open('w') as f:
                p=subprocess.Popen(['taskset','-c','0-1',fixture,'18431','full','32','64','2'],stdout=f,stderr=subprocess.STDOUT,env={k:v for k,v in os.environ.items() if k not in ('LD_PRELOAD','LD_LIBRARY_PATH')})
                try:
                    for _ in range(50):
                        assert p.poll() is None
                        try:
                            with urllib.request.urlopen(url,timeout=1) as response: assert response.read()==b'0123456789abcdef0123456789abcdef'
                            break
                        except OSError: time.sleep(.1)
                    else: raise RuntimeError('readiness timeout')
                    maps=Path(f'/proc/{p.pid}/maps').read_text()
                    assert {line.split()[-1] for line in maps.splitlines() if 'libcoakka_http_host' in line}=={str(lib)}
                    assert Path(f'/proc/{p.pid}/exe').resolve()==fixture and os.sched_getaffinity(p.pid)=={0,1}
                    assert all(os.sched_getaffinity(int(t.name)) <= {0,1} for t in Path(f'/proc/{p.pid}/task').iterdir())
                    (d/'identity.json').write_text(json.dumps({str(lib):sha(lib),str(fixture):sha(fixture)}))
                    log=(d/'server.log').read_text()
                    assert 'host-batch-config request=64 terminal=32' in log and 'host-execution configured=2 active=2' in log
                    load('warmup',5)
                    assert thermal('pre-timed') and trace[-1]['c']<50, 'timed-load admission requires <50 C'
                    load('timed',10,('--log-file='+str(d/'requests.tsv'),))
                    status=Path(f'/proc/{p.pid}/status').read_text()
                    (d/'process-status.txt').write_text(status)
                finally:
                    if p.poll() is None:
                        p.send_signal(signal.SIGTERM)
                        try: p.wait(timeout=12)
                        except subprocess.TimeoutExpired: p.kill(); p.wait()
                assert p.returncode==0
            lat=[]
            with (d/'requests.tsv').open('rb') as f,gzip.open(d/'requests.tsv.gz','wb') as z:
                for line in f:
                    z.write(line)
                    _,code,us=line.split()
                    assert code==b'200'
                    lat.append(int(us))
            (d/'requests.tsv').unlink()
            log=(d/'timed.log').read_text()
            assert '0 failed, 0 errored, 0 timeout' in log
            lat.sort()
            metric=lambda name:int(re.search(r'^'+name+r':\s+(\d+)',status,re.M)[1])/1024
            rows.append(dict(attempt=attempt,requests=len(lat),req_s=len(lat)/10,p50_ms=lat[math.ceil(len(lat)*.50)-1]/1000,p95_ms=lat[math.ceil(len(lat)*.95)-1]/1000,p99_ms=lat[math.ceil(len(lat)*.99)-1]/1000,rss_mib=metric('VmRSS'),peak_rss_mib=metric('VmHWM'),start_c=next(t['c'] for t in trace if t['phase']=='pre-timed'),max_c=max(t['c'] for t in trace if t['phase'].startswith('timed'))))
            print(rows[-1],flush=True)
        except (AssertionError,RuntimeError) as e:
            (d/'rejected.txt').write_text(repr(e))
            print('rejected',attempt,repr(e),trace[-1],flush=True)
            if trace[-1]['phase'] not in ('pre-timed','timed','timed-end') or (trace[-1]['phase'].startswith('timed') and trace[-1]['throttle']=='throttled=0x0'): raise
        finally:
            governors(old)
            (d/'thermal.json').write_text(json.dumps(trace,indent=2))
finally:
    governors(old)
    restored=[p.read_text().strip() for p in gov]==old
    (out/'summary.json').write_text(json.dumps(dict(rows=rows,governors_restored=restored),indent=2))
assert len(rows)==3 and restored
