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

require 'json'

class LanguagesCpp
  def initialize(string_indices, text_entries, output_directory, remap_identical_texts, generate_binary_language_files)
    @string_indices = string_indices #dictionary of all string indices into the characters array
    @text_entries = text_entries
    @output_directory = output_directory
    @remap_identical_texts = remap_identical_texts
    @generate_binary_language_files = generate_binary_language_files
  end
  def run
    @text_entries.languages.each do |language|
      LanguageXxCpp.new(@string_indices, @text_entries, @output_directory, @remap_identical_texts, @generate_binary_language_files, language).run
    end

    #remove any unused LanguageXX.cpp files
    Dir.glob("#{@output_directory}/src/Language*.cpp").each do |file|
      m = /Language(.*).cpp/.match(file)
      if !@text_entries.languages.include?(m[1])
        File.delete(file) if File.exist?(file)
      end
    end

  end
end

class LanguageXxCpp < Template
  Presenter = Struct.new(:text_id, :int_array)

  def initialize(string_indices, text_entries, output_directory, remap_identical_texts, generate_binary_language_files, language)
    @string_indices = string_indices #dictionary of all string indices into the characters array
    @remap_identical_texts = remap_identical_texts
    @generate_binary_language_files = generate_binary_language_files
    @language = language
    super(text_entries, [], output_directory)
    @cache = {}
  end

  def cache_file
    File.join(@output_directory, "cache/LanguageCpp_#{@language}.cache")
  end
  def output_filename
    File.join(@output_directory, output_path)
  end
  def texts
    @text_entries.entries.map(&:cpp_text_id)
  end
  def run
    #build cache dictionary
    @cache["remap"] = @remap_identical_texts
    @cache["language"] = @language
    @cache["language_index"] = @text_entries.languages.index(@language)
    if remap_strings?
      #save text ids and index
      list = [] #list of index,textid
      entries.each_with_index do |entry, index|
        list[index] = [string_index(entry), entry.text_id]
      end
      @cache["indices"] = list
    else
      #save texts
      texts = []
      entries.each_with_index do |entry, index|
        texts << [entry.text_id, entry.int_array]
      end
      @cache["texts"] = texts
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
      #generate LanguageXX.cpp
      super
    end
  end

  def remap_strings?
    @remap_identical_texts=="yes"
  end

  def generate_binary_files?
    @generate_binary_language_files=="yes"
  end

  def language
    @language
  end

  def entries
    #only generate entries once
    @cached_entries ||=
      begin
        entries = text_entries
        entries = handle_no_entries(entries, "DO_NOT_USE")
        present(entries)
      end
  end

  def entries_texts_const_initialization
    entries.map { |entry| "    #{entry.text_id}_#{language}" }.join(",\n")
  end

  def string_index(entry)
    index = @string_indices[entry.int_array]
    index.to_s
  end

#  def entries_s
#    entries = text_entries.entries_with_1_substitution
#    entries = handle_no_entries(entries, "DO_NOT_USE_S")
#    present(entries)
#  end

# def entries_s_texts_const_initialization
#   entries_s.map { |entry| "#{entry.text_id}_#{language}" }.join(",\n")
# end

# def entries_ss
#   entries = text_entries.entries_with_2_substitutions
#   entries = handle_no_entries(entries, "DO_NOT_USE_SS")
#   present(entries)
# end

# def entries_ss_texts_const_initialization
#   entries_ss.map { |entry| "#{entry.text_id}_#{language}" }.join(",\n")
# end

  def input_path
    File.join(root_dir,'Templates','LanguageXX.cpp.temp')
  end

  def output_path
    "src/Language#{language}.cpp"
  end

  private

  def handle_no_entries(entries, text)
    if entries.empty?
       empty_entry = TextEntry.new(text, "typography")
       empty_entry.add_translation(language, "")
       [empty_entry]
    else
      entries
    end
  end

  def present(entries)
    entries.map do |entry|
      Presenter.new(entry.cpp_text_id, entry.translation_in(language).unicodes) 
    end
  end

end
