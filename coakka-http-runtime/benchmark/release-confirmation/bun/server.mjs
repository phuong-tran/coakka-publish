// Loopback Bun fixture for the release confirmation on Linux.
import fs from 'node:fs';
import crypto from 'node:crypto';
import { fileURLToPath } from 'node:url';

// Verify the package entry and platform implementation before serving traffic.
const entry = fileURLToPath(import.meta.resolve('coakka-http-runtime-bun'));
const api = await import('coakka-http-runtime-bun');
const platform = new URL(
  '../platforms/linux-arm64/src/index.mjs',
  import.meta.resolve('coakka-http-runtime-' + (process.versions.bun ? 'bun' : 'node')),
);
if ((await import(platform.href)).Builder !== api.Builder) {
  throw new Error('loaded connector does not match Linux ARM64 entry');
}
const stopped = new Promise((resolve) => {
  process.once('SIGTERM', resolve);
  process.once('SIGINT', resolve);
});

const { Builder, CpuPolicy, Header, Headers, Limits, NotificationProfile, Response } = api;
const body = Buffer.from('0123456789abcdef0123456789abcdef');
const headers = new Headers([new Header('content-type', 'application/octet-stream')]);

const response = Response.bytes(body, { headers });
const service = new Builder()
  .listen('127.0.0.1', 18430)
  .limits(
    new Limits({
      cpuPolicy: CpuPolicy.AUTO,
      requestNotificationProfile: NotificationProfile.ULTRA,
      terminalNotificationProfile: NotificationProfile.XXLARGE,
    }),
  )
  .get('/fixed', () => response)
  .start();

function identify(kind, path) {
  const resolved = fs.realpathSync(path);
  const digest = crypto.createHash('sha256').update(fs.readFileSync(resolved)).digest('hex');
  console.log(`identity-${kind}=${resolved}\t${digest}`);
}

try {
  // The runner compares these startup records with the prepared artifacts.
  identify('module', entry);
  identify('module', fileURLToPath(platform));
  identify('fixture', fileURLToPath(import.meta.url));

  const mappedImages = fs.readFileSync('/proc/self/maps', 'utf8')
    .split('\n')
    .map((line) => line.slice(line.indexOf('/')))
    .filter((path) => path.includes('coakka_http'));
  for (const image of new Set(mappedImages)) {
    identify('image', image);
  }

  console.log(
    'coakka-runtime-info=' +
      JSON.stringify(service.runtimeInfo(), (_, v) =>
        typeof v === 'bigint' ? v.toString() : v,
      ),
  );
  await stopped;
} finally {
  await service.close();
}
console.log('benchmark-shutdown=pass');
