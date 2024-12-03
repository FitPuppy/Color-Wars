# Nazwa wyjściowego pliku
TARGET = web/ColorWars.html

# Źródłowe pliki C++
SOURCES = main.cpp

# Flagi kompilatora
CXXFLAGS = -std=c++14 -Wall -O2
EMFLAGS = -s ASYNCIFY -s TOTAL_MEMORY=67108864 \
          -s FORCE_FILESYSTEM=1 --shell-file shell.html \
          -s EXPORTED_FUNCTIONS='["_free","_malloc","_main","_cellClicked"]' \
          -s EXPORTED_RUNTIME_METHODS=ccall,cwrap

# Ścieżki do bibliotek (przykładowe ścieżki)
#INCLUDE_PATHS = -I./ -I/path/to/raylib/include
#LIBRARY_PATHS = -L/path/to/raylib/lib

# Biblioteki
#LIBS = /path/to/raylib.a

# Reguła domyślna
all: $(TARGET)

# Reguła kompilacji
$(TARGET): $(SOURCES)
	emcc $(CXXFLAGS) $(EMFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SOURCES) $(LIBS) -o $(TARGET)
	python -m http.server

# Czyszczenie plików tymczasowych
clean:
	rm -f $(TARGET) *.o
