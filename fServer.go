package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	http.HandleFunc("/download/", downloadHandler) // 保持相同的URL模式
	
	port := ":9090"
	fmt.Printf("Server started. Download URLs:\n")
	fmt.Printf("http://localhost%s/download/chatroom-cli.tar\n", port)
	fmt.Printf("http://localhost%s/download/another-file.tar\n", port) // 添加新文件的URL
	log.Fatal(http.ListenAndServe(port, nil))
}

func downloadHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != "GET" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}
	
	// 从URL路径中提取文件名
	requestedFile := strings.TrimPrefix(r.URL.Path, "/download/")
	if requestedFile == "" {
		http.NotFound(w, r)
		return
	}

	// 定义允许下载的文件映射
	allowedFiles := map[string]string{
		"chatroom-cli.tar":  "/image/chatroom-cli.tar",    // 第一个文件
		"run_cli.sh":  "/image/run_cli.sh",    // 第二个文件
	}

	// 检查请求的文件是否在允许列表中
	relativePath, ok := allowedFiles[requestedFile]
	if !ok {
		http.NotFound(w, r)
		return
	}

	// 构建完整路径
	filePath := filepath.Join(filepath.Dir("/"), relativePath)

	// 检查文件是否存在
	if _, err := os.Stat(filePath); os.IsNotExist(err) {
		http.NotFound(w, r)
		return
	}

	// 设置响应头
	w.Header().Set("Content-Disposition", "attachment; filename="+requestedFile)
	w.Header().Set("Content-Type", "application/octet-stream")
	
	// 提供文件下载
	http.ServeFile(w, r, filePath)
}
