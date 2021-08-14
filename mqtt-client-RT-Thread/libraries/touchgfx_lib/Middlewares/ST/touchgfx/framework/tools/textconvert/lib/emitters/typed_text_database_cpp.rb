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

class TypedTextDatabaseCpp < Template
  TypedTextPresenter = Struct.new(:alignment, :direction, :typography)

  def initialize(text_entries, typographies, output_directory, generate_binary_language_files, generate_font_format)
    super(text_entries, typographies, output_directory)
    @generate_binary_language_files = generate_binary_language_files
    @generate_font_format = generate_font_format
    @cache = {}
  end

  def run
    #compute the typed text table once
    compute_typed_texts

    #calculate the cache map
    #first the layout databases
    databases = {}
    layouts.each do |l|
      tts = typed_texts(l)
      tt_db = []
      tts.inject(tt_db) do |a, tt|
        #each text has a font index, alignment and direction
        fontIdx = fontmap["getFont_#{tt.typography.cpp_name}_#{tt.typography.font_size}_#{tt.typography.bpp}bpp"]
        a << [fontIdx, tt.alignment.upcase, tt.direction.upcase]
      end
      databases[l] = tt_db
    end
    #now the list of typed text databases
    language_db_list = []
    text_entries.languages.inject(language_db_list) do |list, lang|
      list << (layouts.find{|l|l==lang}||'DEFAULT');list
    end
    @cache["databases"] = databases
    @cache["database_list"]=language_db_list
    @cache["fonts"] = fontmap
    @cache["generate_font_format"] = @generate_font_format

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

  def typed_texts(layout)
    @typed_texts[layout]
  end

  def compute_typed_texts
    @typed_texts = {}
    layouts.each do |layout|
      @typed_texts[layout] = text_entries.collect do |entry|
        typography_name = entry.typographies[layout] || entry.typography
        typography = typographies.find { |t| t.name == typography_name }
        alignment = entry.alignments[layout] || entry.alignment
        direction = entry.directions[layout] || entry.direction
        TypedTextPresenter.new(alignment, direction, typography);
      end
    end
  end

  def generate_binary_files?
    @generate_binary_language_files=="yes"
  end

  def layouts
    @layouts ||=
      begin
        if text_entries.empty?
          ["DEFAULT"]
        else
          (text_entries.first.typographies.keys + text_entries.first.alignments.keys + text_entries.first.directions.keys << "DEFAULT").uniq
        end
      end
  end

  def fonts
    @fonts ||=
      begin
        typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp) }.uniq.collect do |f|
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

  def font_class_name
    @generate_font_format == "1" ? "UnmappedDataFont" : "GeneratedFont"
  end

  def input_path
    File.join(root_dir,'Templates','TypedTextDatabase.cpp.temp')
  end
  def output_path
    'src/TypedTextDatabase.cpp'
  end
  def cache_file
    File.join(@output_directory, 'cache/TypedTextDatabaseCpp.cache')
  end
  def output_filename
    File.join(@output_directory, output_path)
  end
end
