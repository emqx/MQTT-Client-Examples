##############################################################################
# This file is part of the TouchGFX 4.15.0 distribution.
#
# <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
# All rights reserved.</center></h2>
#
# This software component is licensed by ST under Ultimate Liberty license
# SLA0044, the "License"; You may not use this file except in compliance with
# the License. You may obtain a copy of the License at:
#                             www.st.com/SLA0044
#
##############################################################################

class FontsCpp
  def self.font_convert=(font_convert)
    @@font_convert = font_convert
  end

  def initialize(text_entries, typographies, output_directory, font_asset_path, data_format, generate_binary_font_files, generate_font_format)
    @typographies = typographies
    @output_directory = output_directory
    @font_asset_path = font_asset_path
    @data_format = data_format
    @generate_binary_font_files = generate_binary_font_files
    @generate_font_format = generate_font_format
  end
  def run
    unique_typographies = @typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp, t.fallback_character, t.ellipsis_character) }.uniq

    #remove old Table, Kerning, Font files
    #1. Create a list of font names
    font_names = unique_typographies.collect do |typography|
      "#{typography.cpp_name}_#{typography.font_size}_#{typography.bpp}bpp"
    end
    local_path = "#{@output_directory}/src".gsub('\\','/')

    #2, scan for Kerning files, delete files not using a font in font_names
    Dir["#{local_path}/Kerning_*.cpp"].each do |kerning_file|
      if font_names.none? {|font_name| kerning_file == "#{local_path}/Kerning_#{font_name}.cpp" }
        FileUtils.rm(kerning_file)
      end
    end

    #3, scan for Table files
    Dir["#{local_path}/Table_*.cpp"].each do |table_file|
      if font_names.none? {|font_name| table_file == "#{local_path}/Table_#{font_name}.cpp" }
        FileUtils.rm(table_file)
      end
    end

    #4, scan for Font files, remove unused
    Dir["#{local_path}/Font_*.cpp"].each do |font_file|
      if font_names.none? {|font_name| font_file.match /#{local_path}\/Font_#{font_name}_\d+.cpp/ }
        FileUtils.rm(font_file)
      end
    end

    #5, scan for cache files
    local_path = "#{@output_directory}/cache".gsub('\\','/')
    Dir["#{local_path}/Font_*Cpp.cache"].each do |cache_file|
      if font_names.none? {|font_name| cache_file == "#{local_path}/Font_#{font_name}Cpp.cache" }
        FileUtils.rm(cache_file)
      end
    end

    unique_typographies.sort_by { |t| sprintf("%s %04d %d",t.font_file,t.font_size,t.bpp) }.each do |typography|
      fonts_directory = File.expand_path(@output_directory)
      font_file = File.expand_path("#{@font_asset_path}/#{typography.font_file}")
      font_index = fontmap["getFont_#{typography.cpp_name}_#{typography.font_size}_#{typography.bpp}bpp"]
      fallback_char = typography[:fallback_character]
      fallback_char ||= 0
      ellipsis_char = typography[:ellipsis_character]
      ellipsis_char ||= 0
      byte_align = @data_format.match("A#{typography.bpp}") ? "-ba" : ""
      cmd = "\"#{@@font_convert}\" \
-f \"#{font_file}\" \
-i #{font_index} \
-w #{typography.font_size} \
-r #{typography.font_size} \
-o \"#{fonts_directory}\" \
-c \"#{@output_directory}/UnicodeList#{typography.cpp_name}_#{typography.font_size}_#{typography.bpp}.txt\" \
-n \"#{typography.cpp_name}\" \
-b #{typography.bpp} \
-d #{fallback_char} \
-e #{ellipsis_char} \
-bf #{@generate_binary_font_files} \
-ff #{@generate_font_format} \
#{byte_align}"
      #puts "Command: #{cmd}"
      output = `#{cmd}`
      #puts "FontConverter: #{output}\n"
      if !$?.success?
        puts cmd
        puts output
        raise "Error generating font from #{font_file}"
      elsif output.match(/WARNING/i)
        puts output
      end
    end
  end

  def fonts
    @fonts ||=
      begin
        @typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp) }.uniq.collect do |f|
          "getFont_#{f.cpp_name}_#{f.font_size}_#{f.bpp}bpp"
        end
      end
  end

  def fontmap
    @fontmap ||=
      begin
        @fontmap = Hash.new
        fonts.each_with_index do |f, i|
          fontmap[f] = i
        end
        fontmap
      end
  end

end
