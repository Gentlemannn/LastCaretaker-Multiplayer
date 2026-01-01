# 🕹️ Project: External Multiplayer Mode (DLL Injection)

## 📌 General Overview

This project aims to **create an experimental multiplayer mode for the game The Last Caretaker**  **without modifying the original game project**.

The multiplayer system is implemented for now through:
- **DLL injection**
- **Dynamic memory reading/writing**
- **Peer-to-Peer (P2P) synchronization**

The goal is mainly **technical and experimental**, to prove that a multiplayer layer can be added *from scratch* to a single-player game and also because for now it doesn't have any multiplayer wich would be so cool for this game
In the future the DLL injection system will be gone and a server will be hosted manually on the first player computer, sending the data to the peer

---

## 🎯 Final Project Goals

- Read the local player position (X, Y, Z)
- Synchronize this position with another remote player
- Recreate a synchronized “ghost player”
- Handle respawns, reloads, and save loading
- Remain **fully stable**, even during Unreal transitions
- Avoid **any modification** to the original game files

---

## 🧠 Technical Concept

### 🔹 DLL Injection
- A DLL is injected into the game process
- An internal thread is created (`CreateThread`)
- Code runs alongside the game loop

### 🔹 Memory Reading
- Dynamic pointer chains are used
- Pointer resolution is recalculated continuously
- Memory access is protected against invalid reads (SEH)

### 🔹 Target Architecture

