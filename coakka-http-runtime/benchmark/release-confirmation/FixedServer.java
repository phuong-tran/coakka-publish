import coakka.http.CpuPolicy;
import coakka.http.Header;
import coakka.http.Headers;
import coakka.http.Limits;
import coakka.http.NotificationProfile;
import coakka.http.Response;
import coakka.http.Responses;
import coakka.http.RuntimeInfo;
import coakka.http.Service;
import coakka.http.ServiceBuilder;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.CountDownLatch;

/** Loopback JVM fixture for the release confirmation on Linux. */
public final class FixedServer {
    public static void main(String[] args) throws Exception {
        byte[] body = "0123456789abcdef0123456789abcdef".getBytes(StandardCharsets.US_ASCII);
        Headers headers = Headers.of(new Header("content-type", "application/octet-stream"));
        Response response = Responses.bytes(body, 200, headers);

        Limits limits = new Limits(
                0, 0, 0, 0, 0L, 0L, 0L, 0, 0, 0, 0, 1024,
                NotificationProfile.ULTRA, NotificationProfile.XXLARGE);
        Service service = new ServiceBuilder()
                .listen("127.0.0.1", 18430)
                .cpu(CpuPolicy.AUTO)
                .concurrency(1)
                .limits(limits)
                .get("/fixed", request -> response)
                .start();

        // The runner sends SIGTERM and checks that the service closed cleanly.
        java.lang.Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            try {
                service.close();
                System.out.println("benchmark-shutdown=pass");
            } catch (Exception error) {
                error.printStackTrace();
            }
        }));

        reportLoadedFiles();
        RuntimeInfo info = service.runtimeInfo();
        System.out.println("coakka-runtime-info={\"abiVersion\":" + info.getAbiVersion()
                + ",\"selectedCpuCount\":" + info.getCpu().getSelectedCpuCount()
                + ",\"activeEventLoops\":" + info.getExecution().getActiveEventLoops()
                + ",\"requestNotificationBatchSize\":" + info.getRequestNotificationBatchSize()
                + ",\"terminalNotificationBatchSize\":" + info.getTerminalNotificationBatchSize()
                + "}");
        System.out.println("effective-limits=" + service.effectiveLimits());
        new CountDownLatch(1).await();
    }

    private static void reportLoadedFiles() throws Exception {
        Path connectorJar = codeSource(Service.class);
        Path fixtureClass = codeSource(FixedServer.class).resolve("FixedServer.class");
        identify("module", connectorJar);
        identify("fixture", fixtureClass);

        // The process map identifies the native libraries actually loaded.
        Set<String> mappedImages = new TreeSet<>();
        for (String line : Files.readAllLines(Paths.get("/proc/self/maps"))) {
            int pathStart = line.indexOf('/');
            if (pathStart >= 0 && line.contains("coakka_http")) {
                mappedImages.add(line.substring(pathStart));
            }
        }
        for (String image : mappedImages) {
            identify("image", Paths.get(image));
        }
    }

    private static Path codeSource(Class<?> type) throws Exception {
        return Paths.get(type.getProtectionDomain().getCodeSource().getLocation().toURI());
    }

    private static void identify(String kind, Path path) throws Exception {
        Path resolved = path.toRealPath();
        byte[] digest = MessageDigest.getInstance("SHA-256").digest(Files.readAllBytes(resolved));
        StringBuilder hex = new StringBuilder();
        for (byte value : digest) {
            hex.append(String.format("%02x", value));
        }
        System.out.println("identity-" + kind + "=" + resolved + "\t" + hex);
    }
}
