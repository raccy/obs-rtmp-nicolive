# coding: utf-8

if __FILE__ == $0
  readme_md = ARGV[0]
  install_md = ARGV[1]
  readme_txt = ARGV[2]

  readme_md_all = IO.read(readme_md, mode: "rt", encoding: "UTF-8")
  install_md_all = IO.read(install_md, mode: "rt", encoding: "UTF-8")
  text = ""
  flag = 0
  readme_md_all.each_line do |line|
    case flag
    when 0
      if line =~ /^インストール方法$/
        text << install_md_all
        text << "\n"
        flag = 1
      else
        text << line
      end
    when 1
      if line =~ /^使い方$/
        flag = 2
        text << line
      end
    else
      text << line
    end
  end
  open(readme_txt, "wt:Windows-31J") do |io|
    io.write text
  end
end
