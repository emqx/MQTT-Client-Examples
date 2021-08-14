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

class TextsCpp < Template
  def initialize(characters, text_entries, typographies, output_directory, remap_identical_texts, generate_binary_language_files)
    @characters = characters         #one array of the needed strings in optimal order
    @remap_identical_texts = remap_identical_texts
    @generate_binary_language_files = generate_binary_language_files
    super(text_entries, typographies, output_directory)
    @cache = {}
  end

  def run
    #build @cache and compare with file if exists
    @cache["remap"] = @remap_identical_texts

    if remap_strings?
      #record language list and strings
      @cache["languages"] = countries
      @cache["characters"] = @characters
    else
      #record list of languages only
      @cache["languages"] = countries
    end
    
    new_cache_file = false
    if not File::exists?(cache_file)
      new_cache_file = true
    else
        #cache file exists, compare data with cache file
        old_cache = JSON.parse(File.read(cache_file))
        new_cache_file = (old_cache != @cache)
    end

    if new_cache_file
      #write new cache file
      FileIO.write_file_silent(cache_file, @cache.to_json)
    end

    if (!File::exists?(output_filename)) || new_cache_file
      #generate TypedTextDatabase.cpp
      super
    end
  end

  def remap_strings?
    @remap_identical_texts=="yes"
  end
  def generate_binary_files?
    @generate_binary_language_files=="yes"
  end
  def countries
    text_entries.languages.map { |language| language.capitalize }
  end
  def countries_texts
    if countries.empty?
        "0"
    else
        countries.map{ |country| "texts#{country}" }.join(",\n    ")
    end
  end
  def is_rtl
    text_entries.is_rtl
  end
  def input_path
    File.join(root_dir,'Templates','Texts.cpp.temp')
  end
  def output_path
    'src/Texts.cpp'
  end
  def cache_file
    File.join(@output_directory, 'cache/TextsCpp.cache')
  end
  def output_filename
    File.join(@output_directory, output_path)
  end
  def all_unicodes
    if @characters.length==0
      return "0 // No characters in application"
    end
    text = ""
    offset = 0
    initial_offset = 0
    @characters.inject("") do |txt, i|
      last = (offset == @characters.length-1)
      txt << "0x#{i.to_s(16)}#{last ? '' : ','} "
      offset+=1
      if i==0 #end of current word, change line
        txt << "// @#{initial_offset} \"#{text}\""
        txt << "\n    " unless last
        text = ""
        initial_offset = offset
      else
        if i==2
          text << "<>"
        elsif i>=32 && i <127
          text << i.chr
        else
          text << '?'
        end
      end
      txt
    end
  end
end
