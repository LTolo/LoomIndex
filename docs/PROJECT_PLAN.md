# Projektplan: LoomIndex

**LoomIndex** ist ein hochperformanter, nebenläufiger Web-Crawler, der in modernem C++ (C++20) geschrieben wird.

## Architektur

Die Architektur stützt sich auf folgende modulare Komponenten, die strikt nach RAII-Prinzipien und Memory-Safety in C++ implementiert werden:

1. **CrawlerEngine**: Orkestrierung. Verwaltet den Thread-Pool, initiiert asynchrone HTTP-Abfragen und parst Ergebnisse (HTML/Links).
2. **URLFrontier (Thread-safe)**: Eine blockierende/thread-sichere Warteschlange. Sie kapselt `std::mutex` und `std::condition_variable`. Verhindert Data-Races beim Verteilen von Jobs.
3. **BloomFilter**: O(k)-Komplexität. Eine speichereffiziente Methode, um Duplikate von URLs auszusortieren, bevor sie zur `URLFrontier` hinzugefügt werden.
4. **AsyncFetcher**: Eine auf `libcurl` (speziell `curl_multi`) basierende asynchrone HTTP-Erfassung. Nutzt asynchrone Callbacks anstatt blockierender Downloads, um hunderte Sockets gleichzeitig bedienen zu können.

---

## Verzeichnisstruktur

```
LoomIndex/
├── CMakeLists.txt              # Haupt-CMake (Konfiguration für C++20, cURL, FetchContent GTest)
├── docs/
│   └── PROJECT_PLAN.md         # Diese Datei
├── include/
│   └── LoomIndex/              # Öffentliche C++20 Header-Schnittstellen
│       ├── BloomFilter.hpp
│       ├── URLFrontier.hpp
│       ├── AsyncFetcher.hpp
│       └── CrawlerEngine.hpp
├── src/                        # Implementierungsdateien
│   └── main.cpp
└── tests/                      # GTest Unit-Test-Suite
    ├── CMakeLists.txt
    └── test_main.cpp
```

---

## Coder-Agent Aufgabenliste (Priorisiert)

Diese Liste ist als Arbeitsanweisung für den anschließenden **Coder-Agenten** gedacht.

### Phase 1: Grundlegende Utilities & Datenstrukturen
1. **Thread-Pool Implementierung (Vorbereitung für CrawlerEngine)**
   - Erstelle eine klassische Thread-Pool Klasse, die `std::function<void()>` Tasks entgegennimmt. (Falls die `CrawlerEngine` die Threads nicht direkt als `std::vector<std::thread>` verwalten soll).
   - Fokussiere auf sauberen Shutdown per RAII. 

2. **BloomFilter (`src/BloomFilter.cpp`)**
   - Implementiere den Konstruktor (Initialisierung des `std::vector<bool>` basierend auf optimaler Filter-Größe $m$ und Hash-Anzahl $k$).
   - Benutze moderne Hashing-Algorithmen vorhanden in C++ (z. B. `std::hash<std::string>`) kombiniert mit Double-Hashing Methode, um die $k$ Hashes zu erzeugen ($H_i = H_1 + i \cdot H_2$).
   - Unit-Tests: In `tests/test_bloom_filter.cpp` anlegen. Teste False-Positive Rate und grundlegende Funktionalität.

3. **URLFrontier (`src/URLFrontier.cpp`)**
   - Implementiere `push_if_new` unter Verwendung des internen `BloomFilter`. 
   - Achte auf Thread-Safety beim Zugriff auf den Filter und die interne `std::queue`.
   - Implementiere `pop_wait` mit `cv.wait()`.
   - Unit-Tests: In `tests/test_url_frontier.cpp` anlegen (Multi-Threading Tests, Consumer/Producer Verhalten).

### Phase 2: Netzwerk-Layer (libcurl)
4. **AsyncFetcher (`src/AsyncFetcher.cpp`)**
   - Initialisiere den globalen curl context (falls noch nicht geschehen).
   - Implementiere `curl_multi_init` im Konstruktor.
   - Die `fetch_async()` Methode soll ein `CURL*` easy-handle erstellen, in den multi-handle einbinden und Callback-Zustände hinterlegen (z.B. mittels `CURLOPT_PRIVATE`).
   - Implementiere `perform_io()` welche `curl_multi_perform` aufruft, eventuelle Nachrichten des multi-handles via `curl_multi_info_read` liest und das entsprechende C++ Callback triggert.

### Phase 3: Orchestrierung und Parsing
5. **CrawlerEngine (`src/CrawlerEngine.cpp`)**
   - Verbinde `URLFrontier` und `AsyncFetcher`.
   - Sorge für einen main Event-Loop (`perform_io` triggern). Worker-Threads verarbeiten die geparsten Seiten.
   - Implementiere einfaches Link-Parsing (z.B. Regex `<a href="(.*?)">` für den Anfang). Gefundene Links wieder an `URLFrontier->push_if_new()` senden.

### Phase 4: Fehlerbehandlung & Optimierung
6. **Polishing**
   - Graceful Shutdown bei SIGINT (Strg+C).
   - Memory Leak Tests mit ASan (AddressSanitizer im CMake-Skript optional ergänzen).
   - C++20 Konzepte (Concepts) einbauen, um Template-Fehlermeldungen zu beschränken, wo nötig.
