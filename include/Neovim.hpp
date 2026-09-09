#pragma once

#include "Engine.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <optional>

#ifndef _WIN32
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

// Minimal MessagePack-RPC UI client for nvim --embed. It deliberately handles
// only the redraw events needed for a single line-grid editor surface.
namespace neovim {
enum class ValueKind { Nil, Boolean, Integer, String, Array, Map };
struct Value {
  ValueKind kind = ValueKind::Nil;
  int64_t integer = 0;
  bool boolean = false;
  std::string string;
  std::vector<Value> array;
  std::vector<std::pair<Value, Value>> map;
};

inline bool readUnsigned(const std::vector<char>& data, size_t& offset, size_t bytes, uint64_t& result) {
  if (offset + bytes > data.size()) return false;
  result = 0;
  for (size_t index = 0; index < bytes; ++index) result = (result << 8) | static_cast<unsigned char>(data[offset++]);
  return true;
}

inline bool decode(const std::vector<char>& data, size_t& offset, Value& value) {
  if (offset >= data.size()) return false;
  const unsigned char marker = data[offset++];
  if (marker <= 0x7f) { value.kind = ValueKind::Integer; value.integer = marker; return true; }
  if (marker >= 0xe0) { value.kind = ValueKind::Integer; value.integer = static_cast<int8_t>(marker); return true; }
  if ((marker & 0xe0) == 0xa0) {
    const size_t size = marker & 0x1f;
    if (offset + size > data.size()) return false;
    value.kind = ValueKind::String; value.string.assign(data.data() + offset, size); offset += size; return true;
  }
  const auto readString = [&](size_t size) {
    if (offset + size > data.size()) return false;
    value.kind = ValueKind::String; value.string.assign(data.data() + offset, size); offset += size; return true;
  };
  const auto readItems = [&](size_t size, bool map) {
    value.kind = map ? ValueKind::Map : ValueKind::Array;
    for (size_t index = 0; index < size; ++index) {
      Value key; if (!decode(data, offset, key)) return false;
      if (map) { Value entry; if (!decode(data, offset, entry)) return false; value.map.emplace_back(std::move(key), std::move(entry)); }
      else value.array.push_back(std::move(key));
    }
    return true;
  };
  uint64_t size = 0;
  switch (marker) {
    case 0xc0: value.kind = ValueKind::Nil; return true;
    case 0xc2: value.kind = ValueKind::Boolean; value.boolean = false; return true;
    case 0xc3: value.kind = ValueKind::Boolean; value.boolean = true; return true;
    case 0xcc: if (!readUnsigned(data, offset, 1, size)) return false; value.kind = ValueKind::Integer; value.integer = size; return true;
    case 0xcd: if (!readUnsigned(data, offset, 2, size)) return false; value.kind = ValueKind::Integer; value.integer = size; return true;
    case 0xce: if (!readUnsigned(data, offset, 4, size)) return false; value.kind = ValueKind::Integer; value.integer = size; return true;
    case 0xcf: if (!readUnsigned(data, offset, 8, size)) return false; value.kind = ValueKind::Integer; value.integer = size; return true;
    case 0xd0: if (!readUnsigned(data, offset, 1, size)) return false; value.kind = ValueKind::Integer; value.integer = static_cast<int8_t>(size); return true;
    case 0xd1: if (!readUnsigned(data, offset, 2, size)) return false; value.kind = ValueKind::Integer; value.integer = static_cast<int16_t>(size); return true;
    case 0xd2: if (!readUnsigned(data, offset, 4, size)) return false; value.kind = ValueKind::Integer; value.integer = static_cast<int32_t>(size); return true;
    case 0xd3: if (!readUnsigned(data, offset, 8, size)) return false; value.kind = ValueKind::Integer; value.integer = static_cast<int64_t>(size); return true;
    case 0xd9: if (!readUnsigned(data, offset, 1, size)) return false; return readString(size);
    case 0xda: if (!readUnsigned(data, offset, 2, size)) return false; return readString(size);
    case 0xdb: if (!readUnsigned(data, offset, 4, size)) return false; return readString(size);
    case 0xdc: if (!readUnsigned(data, offset, 2, size)) return false; return readItems(size, false);
    case 0xdd: if (!readUnsigned(data, offset, 4, size)) return false; return readItems(size, false);
    case 0xde: if (!readUnsigned(data, offset, 2, size)) return false; return readItems(size, true);
    case 0xdf: if (!readUnsigned(data, offset, 4, size)) return false; return readItems(size, true);
    case 0xc4: if (!readUnsigned(data, offset, 1, size)) return false; return readString(size);
    case 0xc5: if (!readUnsigned(data, offset, 2, size)) return false; return readString(size);
    case 0xc6: if (!readUnsigned(data, offset, 4, size)) return false; return readString(size);
    case 0xc7: if (!readUnsigned(data, offset, 1, size)) return false; if (offset + size + 1 > data.size()) return false; offset += size + 1; value.kind = ValueKind::Nil; return true;
    case 0xc8: if (!readUnsigned(data, offset, 2, size)) return false; if (offset + size + 1 > data.size()) return false; offset += size + 1; value.kind = ValueKind::Nil; return true;
    case 0xc9: if (!readUnsigned(data, offset, 4, size)) return false; if (offset + size + 1 > data.size()) return false; offset += size + 1; value.kind = ValueKind::Nil; return true;
    case 0xd4: if (offset + 2 > data.size()) return false; offset += 2; value.kind = ValueKind::Nil; return true;
    case 0xd5: if (offset + 3 > data.size()) return false; offset += 3; value.kind = ValueKind::Nil; return true;
    case 0xd6: if (offset + 5 > data.size()) return false; offset += 5; value.kind = ValueKind::Nil; return true;
    case 0xd7: if (offset + 9 > data.size()) return false; offset += 9; value.kind = ValueKind::Nil; return true;
    case 0xd8: if (offset + 17 > data.size()) return false; offset += 17; value.kind = ValueKind::Nil; return true;
    default:
      if ((marker & 0xf0) == 0x90) return readItems(marker & 0x0f, false);
      if ((marker & 0xf0) == 0x80) return readItems(marker & 0x0f, true);
      return false;
  }
}

inline void packUnsigned(std::vector<char>& data, uint64_t value) {
  if (value < 128) data.push_back(static_cast<char>(value));
  else if (value < 256) { data.push_back(static_cast<char>(0xcc)); data.push_back(static_cast<char>(value)); }
  else { data.push_back(static_cast<char>(0xcd)); data.push_back(static_cast<char>(value >> 8)); data.push_back(static_cast<char>(value)); }
}
inline void packArray(std::vector<char>& data, size_t size) { data.push_back(static_cast<char>(0x90 | size)); }
inline void packMap(std::vector<char>& data, size_t size) { data.push_back(static_cast<char>(0x80 | size)); }
inline void packString(std::vector<char>& data, std::string_view text) {
  if (text.size() < 32) data.push_back(static_cast<char>(0xa0 | text.size()));
  else { data.push_back(static_cast<char>(0xd9)); data.push_back(static_cast<char>(text.size())); }
  data.insert(data.end(), text.begin(), text.end());
}
inline void sendBytes(Engine& engine, const std::vector<char>& bytes) {
#ifndef _WIN32
  if (engine.neovimInput >= 0) (void)!write(engine.neovimInput, bytes.data(), bytes.size());
#else
  (void)engine; (void)bytes;
#endif
}
inline void sendInput(Engine& engine, std::string_view keys) {
  std::vector<char> packet; packArray(packet, 4); packUnsigned(packet, 0); packUnsigned(packet, engine.neovimRequestId++);
  packString(packet, "nvim_input"); packArray(packet, 1); packString(packet, keys); sendBytes(engine, packet);
}
inline void resizeGrid(Engine& engine, uint32_t columns, uint32_t rows) {
  engine.neovimColumns = std::max(1u, columns); engine.neovimRows = std::max(1u, rows);
  engine.neovimGrid.assign(static_cast<size_t>(engine.neovimColumns) * engine.neovimRows, {});
}
inline void uiAttach(Engine& engine) {
  std::vector<char> packet; packArray(packet, 4); packUnsigned(packet, 0); packUnsigned(packet, engine.neovimRequestId++);
  packString(packet, "nvim_ui_attach"); packArray(packet, 3); packUnsigned(packet, engine.neovimColumns); packUnsigned(packet, engine.neovimRows);
  packMap(packet, 2); packString(packet, "ext_linegrid"); packet.push_back(static_cast<char>(0xc3)); packString(packet, "rgb"); packet.push_back(static_cast<char>(0xc3)); sendBytes(engine, packet);
}
inline void uiTryResize(Engine& engine, uint32_t columns, uint32_t rows) {
  if (columns == engine.neovimColumns && rows == engine.neovimRows) return;
  resizeGrid(engine, columns, rows);
  std::vector<char> packet; packArray(packet, 4); packUnsigned(packet, 0); packUnsigned(packet, engine.neovimRequestId++);
  packString(packet, "nvim_ui_try_resize"); packArray(packet, 2); packUnsigned(packet, columns); packUnsigned(packet, rows); sendBytes(engine, packet);
}
inline void handleRedraw(Engine& engine, const Value& redraw) {
  for (const Value& event : redraw.array) {
    if (event.kind != ValueKind::Array || event.array.empty() || event.array[0].kind != ValueKind::String) continue;
    const std::string& name = event.array[0].string;
    for (size_t call = 1; call < event.array.size(); ++call) {
      const Value& args = event.array[call]; if (args.kind != ValueKind::Array) continue;
      if (name == "grid_resize" && args.array.size() >= 3) resizeGrid(engine, args.array[1].integer, args.array[2].integer);
      else if (name == "grid_clear") std::fill(engine.neovimGrid.begin(), engine.neovimGrid.end(), NeovimCell{});
      else if (name == "grid_cursor_goto" && args.array.size() >= 3) { engine.neovimCursorRow = args.array[1].integer; engine.neovimCursorColumn = args.array[2].integer; }
      else if (name == "mode_change" && !args.array.empty() && args.array[0].kind == ValueKind::String) engine.neovimMode = args.array[0].string;
      else if (name == "grid_line" && args.array.size() >= 4) {
        uint32_t row = args.array[1].integer, column = args.array[2].integer, highlight = 0;
        if (row >= engine.neovimRows || args.array[3].kind != ValueKind::Array) continue;
        for (const Value& cell : args.array[3].array) {
          if (cell.kind != ValueKind::Array || cell.array.empty()) continue;
          const std::string text = cell.array[0].string;
          if (cell.array.size() > 1 && cell.array[1].kind == ValueKind::Integer) highlight = cell.array[1].integer;
          const uint32_t repeat = cell.array.size() > 2 ? cell.array[2].integer : 1;
          for (uint32_t copy = 0; copy < repeat && column < engine.neovimColumns; ++copy, ++column)
            engine.neovimGrid[static_cast<size_t>(row) * engine.neovimColumns + column] = {text, highlight};
        }
      } else if (name == "hl_attr_define" && args.array.size() >= 2) {
        NeovimHighlight highlight{};
        if (args.array[1].kind == ValueKind::Map) for (const auto& [key, value] : args.array[1].map) {
          if (key.string == "foreground") highlight.foreground = value.integer;
          if (key.string == "background") highlight.background = value.integer;
        }
        engine.neovimHighlights[args.array[0].integer] = highlight;
      }
    }
  }
}
inline void poll(Engine& engine) {
#ifndef _WIN32
  if (engine.neovimOutput < 0) return;
  std::array<char, 8192> buffer{};
  for (;;) { const ssize_t count = read(engine.neovimOutput, buffer.data(), buffer.size()); if (count > 0) engine.neovimBytes.insert(engine.neovimBytes.end(), buffer.data(), buffer.data() + count); else break; }
  size_t consumed = 0;
  while (consumed < engine.neovimBytes.size()) { size_t offset = consumed; Value message; if (!decode(engine.neovimBytes, offset, message)) break; consumed = offset; if (message.kind == ValueKind::Array && message.array.size() >= 3 && message.array[0].integer == 2 && message.array[1].string == "redraw") handleRedraw(engine, message.array[2]); }
  if (consumed) engine.neovimBytes.erase(engine.neovimBytes.begin(), engine.neovimBytes.begin() + consumed);
  int status = 0;
  if (engine.neovimProcess > 0 && waitpid(engine.neovimProcess, &status, WNOHANG) == engine.neovimProcess) {
    close(engine.neovimInput); close(engine.neovimOutput);
    engine.neovimInput = engine.neovimOutput = engine.neovimProcess = -1;
    engine.neovimStarted = false;
    engine.neovimWindowOpen = false;
    engine.neovimShutdownRequested = true;
    engine.neovimStatus = "Neovim closed";
  }
#else
  (void)engine;
#endif
}
inline bool start(Engine& engine) {
  if (engine.neovimStarted) return true;
#ifdef _WIN32
  engine.neovimStatus = "Embedded Neovim is not implemented on Windows yet"; return false;
#else
  int input[2]{}, output[2]{};
  if (pipe(input) || pipe(output)) { engine.neovimStatus = "Cannot create Neovim pipes"; return false; }
  const pid_t process = fork();
  if (process == 0) {
    dup2(input[0], STDIN_FILENO); dup2(output[1], STDOUT_FILENO); close(input[1]); close(output[0]);
    setenv("NVIM_LOG_FILE", "/tmp/blossom-nvim.log", 1);
    execlp("nvim", "nvim", "--embed", "--clean", "-u", NEOVIM_CONFIG_FILE, static_cast<char*>(nullptr));
    _exit(127);
  }
  close(input[0]); close(output[1]);
  if (process < 0) { close(input[1]); close(output[0]); engine.neovimStatus = "Cannot start Neovim"; return false; }
  fcntl(output[0], F_SETFL, fcntl(output[0], F_GETFL) | O_NONBLOCK);
  engine.neovimInput = input[1]; engine.neovimOutput = output[0]; engine.neovimProcess = process; engine.neovimStarted = true;
  resizeGrid(engine, 80, 24); uiAttach(engine); engine.neovimStatus = "Embedded Neovim"; return true;
#endif
}
inline void shutdown(Engine& engine) {
#ifndef _WIN32
  if (engine.neovimInput >= 0) close(engine.neovimInput); if (engine.neovimOutput >= 0) close(engine.neovimOutput);
  if (engine.neovimProcess > 0) { kill(engine.neovimProcess, SIGTERM); waitpid(engine.neovimProcess, nullptr, 0); }
#endif
  engine.neovimInput = engine.neovimOutput = engine.neovimProcess = -1; engine.neovimStarted = false; engine.neovimShutdownRequested = false;
}
} // namespace neovim
