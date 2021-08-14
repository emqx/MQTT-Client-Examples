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

class TextEntries
  include Enumerable

  def initialize
    @entries = []
    @is_rtl = false
    @unicode_is_rtl = false
  end

  def each(&block)
    @entries.each(&block)
  end

  def remove(entry)
    @entries.delete(entry)
  end

  def add(entry)
    @entries.push(entry)
  end

  def empty?
    @entries.empty?
  end

  def different_typographies
    [ar,default]
  end

  def different_alignments
    [ar,default]
  end

  def languages
    if @entries.empty?
      []
    else
      @entries.first.languages
    end
  end

  def remove_language(language)
    @entries.each { |entry| entry.remove_translation_in(language) }
  end

  def typographies
    @entries.map { |entry| entry.typography }.uniq
  end

  def entries
    @entries
  end

  def with_typography(typography)
    @entries.select { |entry| entry.typography == typography }
  end

  def include?(text_entry)
    @entries.find { |entry| entry.text_id == text_entry.text_id || entry.cpp_text_id == text_entry.cpp_text_id }
  end

  def unicode_uses_rtl
    @unicode_is_rtl = true
  end

  def is_rtl
    @unicode_is_rtl || @entries.any? { |entry| entry.is_rtl }
  end

end

class TextEntry
  attr_reader :text_id
  attr_reader :typography
  attr_reader :typographies
  attr_reader :alignments
  attr_reader :directions

  def initialize(text_id, typography, alignment, direction)
    @text_id = text_id
    @typographies = {}
    @alignments = {}
    @directions = {}
    @translations = {}

    # default typography
    @typography = typography

    # default alignment
    @alignment = alignment

    # default direction
    @direction = get_direction_as_string(direction)
    @right_to_left = false
  end

  def add_typography(language, typography)
    @typographies[language] = typography
  end
  def add_alignment(language, alignment)
    @alignments[language] = alignment
  end
  def add_direction(language, direction)
    @directions[language] = direction
  end

  def add_translation(language, text)
    translation = Translation.new(text)
    @translations[language] = translation
  end

  def remove_translation_in(language)
    @translations.delete(language)
  end

  def translations
    @translations.values
  end

  def translation_in(language)
    @translations[language]
  end

  def translations_with_typography(typography)
    languages_with_typography = languages.select do |language|
      if @typographies[language].nil?
        @typography == typography
      else
        @typographies[language] == typography
      end
    end

    languages_with_typography.collect{ |language| translation_in(language) }
  end

  def languages
    @translations.keys
  end

  def number_of_substitutions_in(language)
    @translations[language].number_of_substitutions
  end

  def cpp_text_id
    cppify(text_id)
  end

  def alignment
    get_alignment_as_string(@alignment)
  end

  def direction
    get_direction_as_string(@direction)
  end

  # includes the default typography
  def get_all_typographies
    @typographies.values.compact.insert(0, @typography)
  end

  # includes the default alignment
  def get_all_alignments_as_string
    @alignments.values.compact.collect{ |a| get_alignment_as_string(a) }.insert(0, alignment)
  end

  # includes the default direction
  def get_all_directions_as_string
    @directions.values.compact.collect{ |a| get_direction_as_string(a) }.insert(0, direction)
  end

  def is_rtl
    @is_rtl
  end

  private

  def get_alignment_as_string(a)
    case a.to_s.downcase
    when 'right'
      'RIGHT'
    when 'center'
      'CENTER'
    when 'left', ''
      'LEFT'
    else
      a.to_s
    end
  end

  def get_direction_as_string(d)
    case d.to_s.downcase
    when 'ltr', ''
      'LTR'
    when 'rtl'
      @is_rtl = true
      'RTL'
    else
      d.to_s
    end
  end

  def cppify(text)
    t_type = "T_" + text

    # strip the keys for characters, that can not be used in C++
    t_type = t_type.to_ascii
    t_type.gsub!(" ", "_")
    t_type.gsub!(")", "")
    t_type.gsub!("(", "")
    t_type.gsub!("-", "")
    t_type.gsub!("\"", "")
    t_type.gsub!("/", "")
    t_type.gsub!(".", "")
    t_type
  end
end

class Translation
  def initialize(text)
    @text = text
  end
  def empty?
    @text.nil? || @text.empty?
  end
  def length
    @text.length
  end
  def number_of_substitutions
    to_cpp.count("\2")
  end
  def unicodes
    @unicodes ||=
      begin
        numbers.map { |number| number.to_s.gsub(/\[|\]/,'').to_i }
      end
  end
  def to_cpp
    cpp_text = @text.gsub("\2", '') # Remove all existing placeholders
    regex = Regexp.new(/([^\\]|^)<(|.*?[^\\])>/) # Avoid matching \< and \>
    while cpp_text.match(regex)
      cpp_text.gsub!(regex, '\1'+"\2")
    end
    cpp_text.gsub('\\<', '<').gsub('\\>', '>') # Remove \ before < and >
  end
  private
  def numbers
    to_cpp.unpack('U*')
  end
end


class String
  def to_ascii
    # modernized version of http://craigjolicoeur.com/blog/ruby-iconv-to-the-rescue
    self.encode("ASCII", "UTF-8", :undef => :replace, :invalid => :replace, :replace => '').
      unpack('U*').select { |cp| cp < 127 }.pack('U*')
  end
end
