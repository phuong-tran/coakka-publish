import fs from 'node:fs';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';
const entry=fileURLToPath(import.meta.resolve('coakka-http-runtime-node'));
const api=await import('coakka-http-runtime-node');
const platform = new URL("../platforms/linux-arm64/src/index.mjs", import.meta.resolve("coakka-http-runtime-"+(process.versions.bun?"bun":"node")));
if ((await import(platform.href)).Builder !== api.Builder) throw new Error("loaded connector does not match Linux ARM64 entry");
const stopped=new Promise(r=>{process.once('SIGTERM',r);process.once('SIGINT',r)});
const {Builder,CpuPolicy,Header,Headers,Limits,NotificationProfile,Response}=api;
const body=Buffer.from('0123456789abcdef0123456789abcdef');
const headers=new Headers([new Header('content-type','application/octet-stream')]);
const make=()=>Response.bytes(body,{headers}); const shared=make();
const service=new Builder().listen('127.0.0.1',18430).limits(new Limits({cpuPolicy:CpuPolicy.AUTO,requestNotificationProfile:NotificationProfile.ULTRA,terminalNotificationProfile:NotificationProfile.XXLARGE})).get('/fixed',process.env.RESPONSE_MODE==='fresh'?make:()=>shared).start();
function identify(kind,path){path=fs.realpathSync(path);console.log(`identity-${kind}=${path}\t${crypto.createHash('sha256').update(fs.readFileSync(path)).digest('hex')}`)}
try {
 identify('module',entry);identify('module',fileURLToPath(platform));identify('fixture',fileURLToPath(import.meta.url));
 for(const p of new Set(fs.readFileSync('/proc/self/maps','utf8').split('\n').map(l=>l.slice(l.indexOf('/'))).filter(p=>p.includes('coakka_http'))))identify('image',p);
 console.log('coakka-runtime-info='+JSON.stringify(service.runtimeInfo(),(_,v)=>typeof v==='bigint'?v.toString():v));
 await stopped;
}finally{await service.close()}
console.log('benchmark-shutdown=pass');
