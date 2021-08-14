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

$:.unshift File.dirname(__FILE__)

WINDOWS_LINE_ENDINGS = "\r\n"
UNIX_LINE_ENDINGS = "\n"
#on windows/mingw file.write will it self translate \n to \r\n, on linux not
LINE_ENDINGS = RUBY_PLATFORM.match(/linux/) ? WINDOWS_LINE_ENDINGS : UNIX_LINE_ENDINGS

def root_dir
  # Get the dirname of this (main.rb) file:
  @root_dir ||= File.dirname(__FILE__)
end

class Main
  def self.banner
    <<-BANNER
Create binary and cpp text files from excel translations

Usage: #{File.basename($0)} file.xlsx path/to/fontconvert.out path/to/fonts_output_dir path/to/localization_output_dir path/to/font/asset calling_path {remap|yes|no} {A1|A2|A4|A8} {binary_translations} {binary_fonts} {RGB565|RGB888|BW|GRAY2|GRAY4|ARGB2222|ABGR2222|RGBA2222|BGRA2222}
Where 'remap'/'yes' will map identical texts to the same memory area to save space
      'A1'/'A2'/'A4'/'A8' will generate fonts in the given format
      'binary_translations' will generate binary translations instead of cpp files
      'binary_fonts' will generate binary font files instead of cpp files
      last argument is the framebuffer format (used to limit the bit depth of the generated fonts)
      Configuration specified in the application.config file take precedence over the commandline arguments
BANNER
  end

  def self.missing_files
    return !File.exists?("#{@fonts_output_path}/include/fonts/ApplicationFontProvider.hpp") ||
           !File.exists?("#{@localization_output_path}/include/texts/TextKeysAndLanguages.hpp")
  end

  if __FILE__ == $0

    if ARGV.count < 6
      abort self.banner
    end

    file_name = ARGV.shift
    font_convert_path = ARGV.shift
    @fonts_output_path = ARGV.shift
    @localization_output_path = ARGV.shift
    font_asset_path = ARGV.shift
    $calling_path = ARGV.shift

    #optional arguments
    if ARGV.include?("yes") or ARGV.include?("remap")
      remap_identical_texts = "yes"
    end

    data_format = ""
    if ARGV.include?("A1")
      data_format += "A1"
    end
    if ARGV.include?("A2")
      data_format += "A2"
    end
    if ARGV.include?("A4")
      data_format += "A4"
    end
    if ARGV.include?("A8")
      data_format += "A8"
    end

    generate_binary_language_files = ""
    if ARGV.include?("binary_translations")
      generate_binary_language_files = "yes"
      if remap_identical_texts == "yes"
        puts "Disabling remapping of identical texts, because binary language files are generated"
        remap_identical_texts = "no"
      end
    end

    generate_binary_font_files = "no"
    if ARGV.include?("binary_fonts")
      generate_binary_font_files = "yes"
    end

    framebuffer_bpp = ""
    ["BPP32", "BPP24", "BPP16", "BPP8", "BPP4", "BPP2", "BPP1"].each do |format|
      if ARGV.include?(format)
        framebuffer_bpp = format
      end
    end

    require 'fileutils'
    require 'json'
    require 'lib/file_io'

    generate_font_format = "0" # 0 = normal font format, 1 = unmapped_flash_font_format

    application_config = File.join($calling_path, "application.config")
    if File.file?(application_config)
      text_conf = JSON.parse(File.read(application_config))["text_configuration"] || {}

      remap = text_conf["remap"]
      if !remap.nil?
        remap_identical_texts = remap == "yes" ? "yes" : "no"
      end

      a1 = text_conf["a1"]
      if !a1.nil?
        data_format += a1 == "yes" ? "A1" : ""
      end
      a2 = text_conf["a2"]
      if !a2.nil?
        data_format += a2 == "yes" ? "A2" : ""
      end
      a4 = text_conf["a4"]
      if !a4.nil?
        data_format += a4 == "yes" ? "A4" : ""
      end

      binary_translations = text_conf["binary_translations"]
      if !binary_translations.nil?
        generate_binary_language_files = binary_translations == "yes" ? "yes" : ""
        if generate_binary_language_files == "yes" && remap_identical_texts == "yes"
          puts "Disabling remapping of identical texts, because binary language files are generated"
          remap_identical_texts = "no"
        end
      end

      binary_fonts = text_conf["binary_fonts"]
      if !binary_fonts.nil?
        generate_binary_font_files = binary_fonts== "yes" ? "yes" : "no"
      end

      bpp = text_conf["framebuffer_bpp"]
      if !bpp.nil?
        framebuffer_bpp = "BPP" + bpp
      end

      font_format = text_conf["font_format"]
      if !font_format.nil?
        values = ["0", "1"]
        if values.include? font_format
          generate_font_format = font_format
        else
          puts "Font format #{font_format} not correct, using default: \"0\""
        end
      end
    end

    begin
      # 1. if text_converter is newer than compile_time.cache, remove all files under generated/texts and generated/fonts
      # 1b if generated/fonts/include/fonts/ApplicationFontProvider.hpp is missing, force generation of TextKeysAndLanguages.hpp
      # 1c if generated/texts/cache/options.cache contents differ from supplies arguments, force run
      # 2. if generated/texts/cache/compile_time.cache is newer than excel sheet and fonts/ApplicationFontProvider.hpp exists then stop now
      # 3. remove UnicodeList*.txt and CharSizes*.csv
      # 4. create #{@localization_output_path}/include/texts/ and #{@fonts_output_path}/include/fonts/

      # 1:
      text_converter_time = [File.mtime( __FILE__), File.ctime( __FILE__ )].max;

      if ((compile_time_exists = File.exists?("#{@localization_output_path}/cache/compile_time.cache")) && text_converter_time > File.mtime("#{@localization_output_path}/cache/compile_time.cache")) || !compile_time_exists
        #remove all files, as text converter is newer (probably upgraded to new TouchGFX)
        puts "Cleaning generated files from #{@localization_output_path} and #{@fonts_output_path}."
        if @localization_output_path.match /generated\/texts$/
          local_path = @localization_output_path.gsub('\\','/')
          FileUtils.rm_rf("#{local_path}")
        end
        if @fonts_output_path.match /generated\/fonts$/
          local_path = @fonts_output_path.gsub('\\','/')
          FileUtils.rm_rf("#{local_path}")
        end
      end

      # 1b:
      $Force_Generate_TextKeysAndLanguages = self.missing_files

      # 1c:
      force_run = false
      options_file = "#{@localization_output_path}/cache/options.cache"
      options = File.exists?(options_file) && File.read(options_file)

      new_options = ""
      if File.file?(application_config)
        new_options = JSON.parse(File.read(application_config))["text_configuration"]
      else
        new_options = ARGV.to_json
      end

      if(options != new_options)
        force_run = true
        FileIO.write_file_silent(options_file, new_options)
      end

      # 2:
      if File.exists?("#{@localization_output_path}/cache/compile_time.cache") && !self.missing_files && !force_run
        excel_mod_time = [File.mtime(file_name), File.ctime(file_name)].max
        if excel_mod_time < File.mtime("#{@localization_output_path}/cache/compile_time.cache")
          exit
        end
      end

      # 3:
      Dir["#{@fonts_output_path}/UnicodeList*.txt"].each do |text_file|
        FileUtils.rm_f(text_file)
      end
      Dir["#{@fonts_output_path}/CharSizes*.csv"].each do |text_file|
        FileUtils.rm_f(text_file)
      end

      # 4:
      FileUtils.mkdir_p("#{@localization_output_path}/include/texts/")
      FileUtils.mkdir_p("#{@fonts_output_path}/include/fonts")

      require 'rubygems'
      require 'lib/generator'
      require 'lib/emitters/fonts_cpp'
      FontsCpp.font_convert = font_convert_path
      Generator.new.run(file_name, @fonts_output_path, @localization_output_path, font_asset_path, data_format, remap_identical_texts, generate_binary_language_files, generate_binary_font_files, framebuffer_bpp, generate_font_format)
      #touch the cache compile time that we rely on in the makefile
      FileUtils.touch "#{@localization_output_path}/cache/compile_time.cache"

    rescue SystemExit => e

    rescue Exception => e
      STDERR.puts e
      abort "an error occurred in converting texts:\r\n#{e}"
    end
  end
end
