import coakka.http.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.*;
import java.util.concurrent.CountDownLatch;
public final class FixedServer {
 static void identity(String kind,Path p)throws Exception{
  p=p.toRealPath();byte[] hash=MessageDigest.getInstance("SHA-256").digest(Files.readAllBytes(p));StringBuilder h=new StringBuilder();for(byte b:hash)h.append(String.format("%02x",b));System.out.println("identity-"+kind+"="+p+"\t"+h);
 }
 public static void main(String[] args)throws Exception{
  byte[] body="0123456789abcdef0123456789abcdef".getBytes(StandardCharsets.US_ASCII);
  Headers headers=Headers.of(new Header("content-type","application/octet-stream"));
  Response shared=Responses.bytes(body,200,headers);
  boolean fresh="fresh".equals(System.getenv("RESPONSE_MODE"));
  Limits limits=new Limits(0,0,0,0,0L,0L,0L,0,0,0,0,1024,NotificationProfile.ULTRA,NotificationProfile.XXLARGE);
  Service service=new ServiceBuilder().listen("127.0.0.1",18430).cpu(CpuPolicy.AUTO).concurrency(1).limits(limits).get("/fixed",r->fresh?Responses.bytes(body,200,headers):shared).start();
  java.lang.Runtime.getRuntime().addShutdownHook(new Thread(()->{try{service.close();System.out.println("benchmark-shutdown=pass");}catch(Exception e){e.printStackTrace();}}));
  identity("module",Paths.get(Service.class.getProtectionDomain().getCodeSource().getLocation().toURI()));
  identity("fixture",Paths.get(FixedServer.class.getProtectionDomain().getCodeSource().getLocation().toURI()).resolve("FixedServer.class"));
  Set<String> mapped=new TreeSet<>();for(String line:Files.readAllLines(Paths.get("/proc/self/maps"))){int at=line.indexOf('/');if(at>=0&&line.contains("coakka_http"))mapped.add(line.substring(at));}for(String p:mapped)identity("image",Paths.get(p));
  RuntimeInfo i=service.runtimeInfo();System.out.println("coakka-runtime-info={\"abiVersion\":"+i.getAbiVersion()+",\"selectedCpuCount\":"+i.getCpu().getSelectedCpuCount()+",\"activeEventLoops\":"+i.getExecution().getActiveEventLoops()+",\"requestNotificationBatchSize\":"+i.getRequestNotificationBatchSize()+",\"terminalNotificationBatchSize\":"+i.getTerminalNotificationBatchSize()+"}");
  System.out.println("effective-limits="+service.effectiveLimits());
  new CountDownLatch(1).await();
 }
}
