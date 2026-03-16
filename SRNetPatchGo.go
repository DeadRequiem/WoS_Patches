package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"os"
	"strings"
)

const (
	originalURL = "http://www.synthetic-reality.com/synreal.ini"
	patchedURL  = "http://www.reality-synthetic.com/synreal.ini"
)

func main() {
	reader := bufio.NewReader(os.Stdin)

	fmt.Println("=== SRNet.dll Patcher ===")
	fmt.Println()

	// Determine file path — argument or prompt
	var filePath string
	if len(os.Args) > 1 {
		filePath = os.Args[1]
	} else {
		fmt.Print("Enter path to SRNet.dll (or drag and drop it here): ")
		filePath, _ = reader.ReadString('\n')
		filePath = strings.TrimSpace(filePath)
		// Strip surrounding quotes (common with drag-and-drop on Windows)
		filePath = strings.Trim(filePath, `"'`)
	}

	if filePath == "" {
		fmt.Println("Error: No file path provided.")
		pause()
		os.Exit(1)
	}

	// Load file
	fmt.Printf("Loading: %s\n", filePath)
	data, err := os.ReadFile(filePath)
	if err != nil {
		fmt.Printf("Error: Could not read file: %v\n", err)
		pause()
		os.Exit(1)
	}

	// Search for URL
	orig := []byte(originalURL)
	if !bytes.Contains(data, orig) {
		fmt.Println("Error: Original URL not found in file.")
		fmt.Println("       The file may already be patched, or this is the wrong file.")
		pause()
		os.Exit(1)
	}

	// Backup
	backupPath := filePath + ".backup"
	if err := copyFile(filePath, backupPath); err != nil {
		fmt.Printf("Error: Could not create backup: %v\n", err)
		pause()
		os.Exit(1)
	}
	fmt.Printf("Backup created: %s\n", backupPath)

	// Patch
	patched := bytes.Replace(data, orig, []byte(patchedURL), 1)

	// Save
	if err := os.WriteFile(filePath, patched, 0644); err != nil {
		fmt.Printf("Error: Could not write patched file: %v\n", err)
		fmt.Println("Tip: Try running as Administrator.")
		pause()
		os.Exit(1)
	}

	fmt.Println()
	fmt.Println("Patched successfully!")
	pause()
}

func copyFile(src, dst string) error {
	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()

	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer out.Close()

	_, err = io.Copy(out, in)
	return err
}

// pause keeps the window open when double-clicked
func pause() {
	fmt.Print("\nPress Enter to exit...")
	bufio.NewReader(os.Stdin).ReadString('\n')
}