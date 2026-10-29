# CoAkka Logger

Logs are useful during an incident, but logging is also work the application
has to perform. Message construction, bursts of traffic and slow output can
affect the requests or jobs the application is trying to serve.

CoAkka Logger provides bounded log storage, explicit acceptance or refusal and
counters across host languages. Use it when log volume, output delays or
unclear record loss have become application concerns. The application can see
pressure and decide how to consume records and handle refused writes.

Logger is independent of CoAkka Runtime and CoAkka HTTP Runtime. Read the
[product documentation](../docs/coakka-logger/README.md) for the full guide.

## Why Logging Affects The Application

A log call does work before a line reaches its destination. Building messages,
serializing objects and capturing diagnostics consume CPU. Temporary strings
and buffers use memory and can add garbage-collection work in managed hosts.
Shared locks can introduce contention. Synchronous file, console or collector
writes can make a request, event loop or worker wait for output.

Moving output to another worker changes where that work runs; it does not
remove formatting, allocation or I/O cost. If producers stay faster than the
consumer, an unbounded queue grows. A bounded queue instead reaches its limit,
where the application must observe rejection or loss. Increasing capacity can
absorb a burst, but consumes memory and does not make a slow destination faster.

CoAkka makes those boundaries observable. It does not promise zero cost,
unlimited throughput, or delivery of every record.

## Queue, Consumer And Sink Are Different Boundaries

The basic language samples use **manual drain**: the application reads accepted
records through its connector and decides what to do with them. Starting that
logger does not start a file writer or a remote collector. The native Logger
also provides file, console and combined sink modes with a writer worker; use
only the modes exposed by the exact package you select.

| Logger owns | Application owns |
| --- | --- |
| Record and queue bounds, admission, level filtering and authoritative counters. | What to log, redaction, correlation, sampling and handling refused writes. |
| Queue reads and the lifecycle of its configured managed sinks. | Manual-drain consumption and any application-provided sink or collector. |
| Supported local file limits and sink-failure reporting. | Storage/collector operations, retention requirements and deployment monitoring. |

Admission still involves work and synchronization. A writer worker does not
remove contention or give every logging call a fixed latency bound. An
application that drains and writes logs on a request thread can still stall
that request. App-created messages and external sink buffers need their own
limits even when Logger's queue is bounded.

## Read The Counters Precisely

The Node.js samples expose these fields through `logger.stats()`:

| Field | Meaning |
| --- | --- |
| `queueCapacity`, `queueDepth`, `queueHighWatermark` | Configured record limit, current queued records and peak queue depth for the instance. |
| `emittedCount` | Records accepted into the native queue. |
| `deliveredCount` | Records removed from the queue by a reader or writer. This precedes managed sink I/O. |
| `droppedCount` | Drops accounted by Logger, including queue-full rejection; native filtering and pressure policies can also contribute. |

Use the call's return/error as well as counters. A connector can skip a disabled
call before native admission, so native counters are not a count of every
application logging attempt. Where the selected API exposes sink-failure and
pressure counters, monitor those too. A rising queue and high-water mark point
to pressure; a drained queue alone does not prove successful output.

Acceptance, dequeue, successful sink write and durable storage are distinct.
These counters do not acknowledge a remote collector or promise persistence
across a crash or power loss. Logger is not a durable audit journal. Choose a
storage protocol with the required acknowledgement and durability contract for
records that must survive those failures.

Stop producers before closing their logger. Manual-drain consumers must read
the records they need before destruction; unread records are not persisted by
`close()`. Managed-sink shutdown joins its writer, so queued work and a slow
destination can affect shutdown time. Observe sink failures rather than
assuming every accepted record reached its destination.

## Try The Boundary

From a CoAkka Samples checkout:

```sh
bash run.sh coakka-logger node basic
bash run.sh coakka-logger node pressure
```

The [pressure sample](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-logger/node/pressure/README.md)
uses a manual-drain queue of two records and eight enabled writes. It accepts
two, rejects six, then reads the accepted records. Its final counters are
`emitted=2 delivered=2 dropped=6`; this demonstrates the admission boundary,
not production capacity or durable delivery.

Keep an existing framework logger when its filtering, queue bounds, output,
shutdown and loss reporting already meet the application's needs. CoAkka is
useful when those contracts need to stay explicit across language hosts; it
does not require replacing a working logging stack or collector.

## Packages And Guides

- [Current package coordinates](../docs/common/current-packages.md)
- [Ecosystem overview](../docs/common/ecosystem-overview.md)
- [Logger samples and operating guidance](https://github.com/phuong-tran/coakka-samples/blob/main/coakka-logger/README.md)
- [Published native Logger catalog](../logger/native/README.md)
- [Published JVM Logger catalog](../logger/jvm/README.md)
- [Published Node.js Logger catalog](../logger/node/README.md)

Use the selected language guide for its runtime, OS, package and sink support.
See [Product Layout](../docs/common/product-layout.md) for stable artifact paths.
Private candidates remain separate from published packages.
