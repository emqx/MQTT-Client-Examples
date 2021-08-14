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

class LanguagesBin
  def initialize(text_entries, typographies, output_directory)
    @text_entries = text_entries
    @typographies = typographies
    @output_directory = output_directory
  end
  def run
    #remove_old_binary_files

    @text_entries.languages.each do |language|
      LanguageXxBin.new(@text_entries, @typographies, @output_directory, language).run
    end
  end

  private

  def remove_old_binary_files
    # Remove any old bin file
    local_path = @output_directory.gsub('\\','/')
    Dir["#{local_path}/binary/Language*.bin"].each do |language_bin_file|
      puts "Deleting #{language_bin_file}"
      FileUtils.rm(language_bin_file)
    end
  end
end

class LanguageXxBin < Template
  Presenter = Struct.new(:text_id, :unicodes)
  LanguageHeader = Struct.new(:offset_to_texts, :offset_to_indices, :offset_to_typedtext)
  TypedTextPresenter = Struct.new(:alignment, :direction, :typography)

  ALIGNMENT = { "LEFT" => 0, "CENTER" => 1, "RIGHT" => 2 }
  TEXT_DIRECTION = { "LTR" => 0, "RTL" => 1 }

  def initialize(text_entries, typographies, output_directory, language)
    @language = language
    @typographies = typographies
    @text_entries = text_entries
    @output_directory = output_directory
    @cache = {}
  end

  def cache_file
    File.join(@output_directory, "cache/LanguageBin_#{@language}.cache")
  end

  def alignment_to_value(alignment)
    ALIGNMENT[alignment.to_s]
  end

  def text_direction_to_value(direction)
    TEXT_DIRECTION[direction.to_s]
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

  def input_path
    File.join(@output_directory, output_path)
  end

  def output_path
    "binary/Language#{language}.bin"
  end

  def typed_texts(language)
    text_entries.collect do |entry|
        typography_name = entry.typographies[language] || entry.typography
        typography = typographies.find { |t| t.name == typography_name }
        alignment = entry.alignments[language] || entry.alignment
        direction = entry.directions[language] || entry.direction
        TypedTextPresenter.new(alignment, direction, typography);
    end
  end

  def typed_texts_(language)
    typed_text_str = typed_texts(language)
    puts "typed_text_str = #{typed_text_str}"
  end

  def fonts
   typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp) }.uniq.collect do |f|
      "getFont_#{f.cpp_name}_#{f.font_size}_#{f.bpp}bpp"
    end
  end

  def fontmap
    fontmap = Hash.new
    fonts.each_with_index do |f, i|
      fontmap[f] = i
    end
    fontmap
  end

  def header(entries)
    nb_entries = 0
    header_struct_size = 12
    header_unicodes_size = 0;
    offset_to_texts = 0
    offset_to_indices = 0
    offset_to_typedtext = 0
    entries.each do |entry|
      nb_entries += 1
      entry.unicodes.split(', ').each { |c|
          header_unicodes_size += 2
      }
    end
    offset_to_texts = header_struct_size
    offset_to_indices = ((offset_to_texts + header_unicodes_size + 3) &~ 0x3)
    offset_to_typedtext = ((offset_to_indices + (4 * nb_entries) + 3) &~ 0x3)
    # puts "Number of Entries     = #{nb_entries}"
    # puts "Header size           = #{header_struct_size}"
    # puts "Unicodes size         = #{header_unicodes_size}"
    # puts "Text Offset           = #{offset_to_texts}"
    # puts "Indices Offset        = #{offset_to_indices}"
    # puts "TypedText Offset      = #{offset_to_typedtext}"
    LanguageHeader.new('0x' + offset_to_texts.to_s(16), '0x' + offset_to_indices.to_s(16), '0x' + offset_to_typedtext.to_s(16))
  end

  def output_filename
    File.join(@output_directory, output_path)
  end

  def run
   #build cache dictionary
    @cache["typographies"] = typographies.collect{|t| [t.name, t.font_file, t.font_size, t.bpp] }
    @cache["language"] = @language
    @cache["language_index"] = @text_entries.languages.index(@language)
    list = [] #list of index,textid
    entries.each_with_index do |entry, index|
      list[index] = [entry.unicodes, entry.text_id]
    end
    @cache["indices"] = list

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
      #generate LanguageXX.bin
      FileUtils.mkdir_p(File.dirname(input_path))
      callingPath = Pathname.new($calling_path)
      filePath = Pathname.new(input_path)
      puts "Generating #{filePath.relative_path_from(callingPath)}"
      File.open(input_path,'wb') do |f|
        # create indices array
        indices_arr = []

        # Writing Language Header
        lang_header = header(entries)
        lang_header.each { |c|
            f.write [c.to_i(16)].pack("L")
        }

        # Writing Texts data
        indices_arr.clear
        indices_arr << 0 # first element is @ offset zero
        nb_data_in_entry = 0
        entries.each do |entry|
          #puts "All Unicodes #{entry.unicodes}"
          entry.unicodes.split(', ').each { |c|
              f.write [c.to_i(16)].pack("S")
              nb_data_in_entry += 1
          }
          indices_arr << nb_data_in_entry #populate the indices array
        end

        # Add padding to align on word size  for next indeces data writing
        loop do
          if Integer(f.pos) == Integer(lang_header.offset_to_indices)
            break
          end
          f.write ["0x00".to_i(16)].pack("S")
        end

        # Remove last indice
        indices_arr.pop

        # Writing Indices
        indices_arr.each { |idx| f.write [idx].pack("L") }

        # Add padding to align on word size for next typed_text data writing
        loop do
          if Integer(f.pos) == Integer(lang_header.offset_to_typedtext)
            break
          end
          f.write ["0x00".to_i(16)].pack("S")
        end

        # Create and Fill TypedTextsData Array
        typed_text_arr = []
        if typed_texts(language).empty?
          # puts "    { #{0}, #{alignment_to_value("LEFT")}, #{text_direction_to_value("LTR")} }"
          typed_text_arr << 0 << alignment_to_value("LEFT") << text_direction_to_value("LTR")
        else
          typed_texts(language).map do |typed_text|
            fontIdx = fontmap["getFont_#{typed_text.typography.cpp_name}_#{typed_text.typography.font_size}_#{typed_text.typography.bpp}bpp"]
            alignment = alignment_to_value(typed_text.alignment.upcase)
            direction = text_direction_to_value(typed_text.direction.upcase)
            # puts "Font Index     --> #{fontIdx}"
            # puts "Alignment      --> #{typed_text.alignment.upcase}"
            # puts "Text Direction --> #{typed_text.direction.upcase}"
            # puts "    { #{fontIdx}, #{alignment_to_value(typed_text.alignment.upcase)}, #{text_direction_to_value(typed_text.direction.upcase)} }"
            combined = direction.to_s(2).to_i(2) * 4 + alignment.to_s(2).to_i(2)
            typed_text_arr << fontIdx << combined
          end
        end

        # Writing TypedTextsData
        typed_text_arr.each do |idx|
            f.write [idx].pack("C")
        end

        # # Add padding to align the binary file size on word size
        loop do
          if ((f.pos & 0x3) == 0)
            break
          end
          f.write ["0x00".to_i(16)].pack("C")
        end
      end
    end
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
      Presenter.new(entry.cpp_text_id, ( entry.translation_in(language).unicodes.map { |u| '0x' + u.to_s(16) } << '0x0' ) .join(', ') )
    end
  end
end
