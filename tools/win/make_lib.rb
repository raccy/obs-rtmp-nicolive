# coding: utf-8
# dllからlibを作ります。

# VS2013 と mingw32-pexports が必要です。
# インストールしたディレクトリにあわせて書き換えて下さい。
VS2013_DIR = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0"
MINGW_DIR = "C:\\MinGW"

DLL_LIST = %w|
  obs
  Qt5Core
  Qt5Widgets
  Qt5Network
  Qt5Gui
|

if $0 == __FILE__
  if ARGV.size == 0
    puts "使い方: ruby make_lib.rb OBSインストール先のディレクトリ"
    exit
  end

  obs_dir = ARGV[0]

  unless FileTest.executable?("#{obs_dir}\\bin\\32bit\\obs32.exe")
    puts "OBSが見つかりません。インストール先を指定して下さい。"
    exit 1
  end

  out_dir = "winlibs"
  out_dir_32bit = "#{out_dir}\\32bit"
  out_dir_64bit = "#{out_dir}\\64bit"
  unless FileTest.directory?(out_dir)
    Dir.mkdir(out_dir)
  end
  unless FileTest.directory?(out_dir_32bit)
    Dir.mkdir(out_dir_32bit)
  end
  unless FileTest.directory?(out_dir_64bit)
    Dir.mkdir(out_dir_64bit)
  end

  msvs12_bin = "#{VS2013_DIR}\\VC\\bin"
  mingw_bin = "#{MINGW_DIR}\\bin"
  pexports_exe = "#{mingw_bin}\\pexports.exe"
  # dumpbin_exe = "#{msvs12_bin}\\dumpbin.exe"
  lib_exe = "#{msvs12_bin}\\lib.exe"
  unless FileTest.executable?(pexports_exe)
    puts "pexports.exeが見つかりません。Ming32のming32-pexportsを標準の場所にインストールしていますか？"
    exit 1
  end
  # unless FileTest.executable?(dumpbin_exe)
  #   puts "dumpbin.exeが見つかりません。VS2013を標準の場所にインストールしていますか？"
  #   exit 1
  # end
  unless FileTest.executable?(lib_exe)
    puts "lib.exeが見つかりません。VS2013を標準の場所にインストールしていますか？"
    exit 1
  end

  DLL_LIST.each do |name|
    `"#{pexports_exe}" /EXPORTS "#{obs_dir}\\bin\\32bit\\#{name}.dll" > "#{out_dir_32bit}\\#{name}.def"`
    `"#{pexports_exe}" /EXPORTS "#{obs_dir}\\bin\\64bit\\#{name}.dll" > "#{out_dir_64bit}\\#{name}.def"`
    `"#{lib_exe}" /def:"#{out_dir_32bit}\\#{name}.def" /out:"#{out_dir_32bit}\\#{name}.lib"`
    `"#{lib_exe}" /MACHINE:x64 /def:"#{out_dir_64bit}\\#{name}.def" /out:"#{out_dir_64bit}\\#{name}.lib"`
  end
end
