// Command fixed-server serves the release-confirmation response through CoAkka.
package main

import (
	"crypto/sha256"
	"encoding/json"
	"errors"
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"runtime/debug"
	"runtime/pprof"
	"strconv"
	"strings"
	"syscall"

	coakkahttp "github.com/phuong-tran/coakka-http-runtime-go/v2"
)

var body = []byte("0123456789abcdef0123456789abcdef")

func main() {
	if len(os.Args) != 3 || os.Args[1] != "coakka" {
		fatal(fmt.Errorf("usage: fixed-server coakka <port>"))
	}
	port, err := strconv.Atoi(os.Args[2])
	if err != nil || port < 1 || port > 65535 {
		fatal(fmt.Errorf("invalid port"))
	}
	runCoakka(uint16(port))
}

func runCoakka(port uint16) {
	stopping := make(chan os.Signal, 1)
	signal.Notify(stopping, os.Interrupt, syscall.SIGTERM)
	defer signal.Stop(stopping)
	policy := coakkahttp.CPUAuto
	switch os.Getenv("COAKKA_BENCH_CPU_POLICY") {
	case "single":
		policy = coakkahttp.CPUSingle
	case "auto":
	default:
		fatal(fmt.Errorf("explicit benchmark CPU intent required"))
	}
	contentType, err := coakkahttp.NewHeader("content-type", "application/octet-stream")
	if err != nil {
		fatal(err)
	}
	headers, err := coakkahttp.NewHeaders(contentType)
	if err != nil {
		fatal(err)
	}
	response, err := coakkahttp.NewResponse(http.StatusOK, headers, body)
	if err != nil {
		fatal(err)
	}
	service, err := coakkahttp.NewBuilder().Listen("127.0.0.1", port).
		CPU(policy).
		Limits(coakkahttp.Limits{
			Notifications: coakkahttp.NotificationProfiles{
				Request:  coakkahttp.NotificationUltra,
				Terminal: coakkahttp.NotificationXXLarge,
			},
		}).
		Get("/fixed", func(*coakkahttp.Request) (coakkahttp.Response, error) {
			return response, nil
		}).Start()
	if err != nil {
		fatal(err)
	}
	// Report the executable, linked module version, and mapped SDK before load.
	exe, _ := os.Executable()
	identity("fixture", exe)
	bi, ok := debug.ReadBuildInfo()
	if !ok {
		fatal(fmt.Errorf("build identity unavailable"))
	}
	for _, d := range bi.Deps {
		if d.Path == "github.com/phuong-tran/coakka-http-runtime-go/v2" {
			if d.Replace != nil {
				fatal(fmt.Errorf("module replacement forbidden"))
			}
			fmt.Printf("identity-module-version=%s\n", d.Version)
		}
	}
	maps, e := os.ReadFile("/proc/self/maps")
	if e != nil {
		fatal(e)
	}
	seen := map[string]bool{}
	for _, l := range strings.Split(string(maps), "\n") {
		at := strings.Index(l, "/")
		if at >= 0 && strings.Contains(l, "coakka_http") {
			path := l[at:]
			if !seen[path] {
				identity("image", path)
				seen[path] = true
			}
		}
	}
	// Report the settings accepted by the running service.
	info, err := service.RuntimeInfo()
	if err != nil {
		fatal(errors.Join(err, service.Close()))
	}
	if info.CPU.SelectedCPUCount > 2 {
		fatal(errors.Join(fmt.Errorf("invalid Core CPU observation"), service.Close()))
	}
	if info.EffectiveIOBackend != coakkahttp.IOPlatformDefault && info.EffectiveIOBackend != coakkahttp.IOUring {
		fatal(errors.Join(fmt.Errorf("unknown Core backend observation"), service.Close()))
	}
	record, err := json.Marshal(map[string]any{
		"cpu": map[string]any{
			"requestedPolicy":  info.CPU.RequestedPolicy,
			"placement":        info.CPU.Placement,
			"selectedCpuCount": info.CPU.SelectedCPUCount,
			"selectedCpuIds":   info.CPU.SelectedCPUIDs[:info.CPU.SelectedCPUCount],
		},
		"execution": map[string]any{
			"observed":             info.Execution.Observed,
			"selection":            info.Execution.Selection,
			"configuredEventLoops": info.Execution.ConfiguredEventLoops,
			"activeEventLoops":     info.Execution.ActiveEventLoops,
		},
		"limits":                        info.Limits,
		"effectiveIoBackend":            info.EffectiveIOBackend,
		"ioUringEffective":              info.EffectiveIOBackend == coakkahttp.IOUring,
		"requestNotificationBatchSize":  info.RequestNotificationBatchSize,
		"terminalNotificationBatchSize": info.TerminalNotificationBatchSize,
	})
	if err != nil {
		fatal(errors.Join(err, service.Close()))
	}
	fmt.Printf("coakka-runtime-info=%s\n", record)
	var profile *os.File
	if path := os.Getenv("COAKKA_CPU_PROFILE"); path != "" {
		profile, err = os.Create(path)
		if err != nil {
			fatal(errors.Join(err, service.Close()))
		}
		if err = pprof.StartCPUProfile(profile); err != nil {
			_ = profile.Close()
			fatal(errors.Join(err, service.Close()))
		}
	}
	<-stopping
	if profile != nil {
		pprof.StopCPUProfile()
		if err = profile.Close(); err != nil {
			fatal(errors.Join(err, service.Close()))
		}
	}
	if err = service.Close(); err != nil {
		fatal(err)
	}
	fmt.Println("benchmark-shutdown=pass")
}

func fatal(err error) {
	fmt.Fprintln(os.Stderr, err)
	os.Exit(1)
}

func identity(kind, path string) {
	path, err := filepath.EvalSymlinks(path)
	if err != nil {
		fatal(err)
	}
	b, err := os.ReadFile(path)
	if err != nil {
		fatal(err)
	}
	fmt.Printf("identity-%s=%s\t%x\n", kind, path, sha256.Sum256(b))
}
