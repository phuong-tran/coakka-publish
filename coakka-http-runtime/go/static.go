package coakkahttp

import (
	"bytes"
	"crypto/sha256"
	"errors"
	"fmt"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

type StaticContent struct {
	Root        string
	Mount       string
	IndexFile   string
	SPAFallback bool
}

func (content StaticContent) validate() error {
	if content.Root == "" {
		return errors.New("coakka http static root is empty")
	}
	if content.Mount == "" {
		content.Mount = "/"
	}
	if content.Mount[0] != '/' || strings.Contains(content.Mount, "..") {
		return errors.New("coakka http static mount is invalid")
	}
	details, err := os.Stat(content.Root)
	if err != nil || !details.IsDir() {
		return errors.New("coakka http static root is not a directory")
	}
	return nil
}

func (content *StaticContent) serve(writer http.ResponseWriter, request *http.Request, bounds Bounds) bool {
	if request.Method != http.MethodGet && request.Method != http.MethodHead {
		return false
	}
	mount := content.Mount
	if mount == "" {
		mount = "/"
	}
	if !strings.HasPrefix(request.URL.Path, mount) ||
		(mount != "/" && request.URL.Path != mount && !strings.HasPrefix(request.URL.Path, mount+"/")) {
		return false
	}
	relative := strings.TrimPrefix(request.URL.Path, mount)
	relative = strings.TrimPrefix(relative, "/")
	index := content.IndexFile
	if index == "" {
		index = "index.html"
	}
	if relative == "" || strings.HasSuffix(request.URL.Path, "/") {
		relative = filepath.Join(relative, index)
	}
	if !safeRelativePath(relative) {
		http.NotFound(writer, request)
		return true
	}
	path := filepath.Join(content.Root, filepath.FromSlash(relative))
	data, details, err := readStaticFile(content.Root, path, bounds.MaxResponseBodyBytes)
	if err != nil && content.SPAFallback && strings.Contains(request.Header.Get("Accept"), "text/html") {
		path = filepath.Join(content.Root, index)
		data, details, err = readStaticFile(content.Root, path, bounds.MaxResponseBodyBytes)
	}
	if err != nil {
		http.NotFound(writer, request)
		return true
	}
	digest := sha256.Sum256(data)
	etag := fmt.Sprintf("\"%x\"", digest[:])
	writer.Header().Set("ETag", etag)
	if request.Header.Get("If-None-Match") == etag {
		writer.WriteHeader(http.StatusNotModified)
		return true
	}
	http.ServeContent(writer, request, filepath.Base(path), details.ModTime(), bytes.NewReader(data))
	return true
}

func readStaticFile(root, path string, maximum int64) ([]byte, os.FileInfo, error) {
	resolvedRoot, err := filepath.EvalSymlinks(root)
	if err != nil {
		return nil, nil, errors.New("static root could not be resolved")
	}
	resolvedPath, err := filepath.EvalSymlinks(path)
	if err != nil {
		return nil, nil, errors.New("static file could not be resolved")
	}
	relative, err := filepath.Rel(resolvedRoot, resolvedPath)
	if err != nil || relative == ".." || strings.HasPrefix(relative, ".."+string(filepath.Separator)) {
		return nil, nil, errors.New("static file is outside its root")
	}
	path = resolvedPath
	details, err := os.Lstat(path)
	if err != nil || !details.Mode().IsRegular() || details.Mode()&os.ModeSymlink != 0 || details.Size() > maximum {
		return nil, nil, errors.New("static file is outside its bound")
	}
	data, err := os.ReadFile(path)
	if err != nil || int64(len(data)) > maximum {
		return nil, nil, errors.New("static file could not be read")
	}
	return data, details, nil
}

func safeRelativePath(value string) bool {
	if value == "" || filepath.IsAbs(value) || strings.Contains(value, "\\") {
		return false
	}
	clean := filepath.Clean(value)
	return clean != "." && clean != ".." && !strings.HasPrefix(clean, ".."+string(filepath.Separator))
}
