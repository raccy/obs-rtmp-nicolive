#!/usr/bin/ruby
# coding: utf-8

class FixRpath
  FIX_RPATH_LIST = ['@loader_path/', '@executable_path/']

  def initialize(path)
    @path = path
    @load_list = []
    @rpath_list = []
    read_otool
  end

  def read_otool
    otool_str = `otool -l #{@path}`
    cmd_type = nil
    otool_str.each_line do |line|
      case line
      when /^\s+cmd\s+(\S+)/
        cmd_type = $1
      when /^\s+name\s+(\S+)/
        name = $1
        if cmd_type == "LC_LOAD_DYLIB"
          @load_list << name
        end
      when /^\s+path\s+(\S+)/
        path = $1
        if cmd_type ==  "LC_RPATH"
          @rpath_list << path
        end
      end
    end
    # p @load_list
    # p @rpath_list
  end

  def fix_load
    @load_list.each do |e|
      if e =~ %r{^/usr/(?!local/)|^/System/|^/Library/}
        next
      end
      name = File.basename(e)
      system("install_name_tool -change #{e} @rpath/#{name} #{@path}")
    end
  end

  def fix_rpath
    @rpath_list.each do |e|
      system("install_name_tool -delete_rpath #{e} #{@path}")
    end
    FixRpath::FIX_RPATH_LIST.each do |e|
      system("install_name_tool -add_rpath #{e} #{@path}")
    end
  end

  def fix_all
    fix_load
    fix_rpath
  end
end

if __FILE__ == $0
  if ARGV.size < 0
    puts "Usage: #{$0} path"
    exit 1
  end
  path = ARGV[0]
  if File.file?(path)
    FixRpath.new(path).fix_all
  else
    fail "Not found path"
  end
end
