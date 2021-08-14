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

require 'lib/excel_reader'

class TextEntriesExcelReader
  attr_reader :reader

  def initialize(file_name)
    #puts "Running TextEntriesExcelReader:init, #{Time.now.strftime("%H:%M:%S:%L")}"
    header_row_number = 3
    header_column_number = 2
    @reader = ExcelReader.new(file_name, "Translation", header_row_number, header_column_number)
    @text_entries = TextEntries.new
  end

  def run
    #puts "Running TextEntriesExcelReader:run, #{Time.now.strftime("%H:%M:%S:%L")}"
    reader.read_header do |header|
      @alignments = header.select { |column| column.match(/^.*-ALIGNMENT$/i) }.map(&:capitalize)
      @directions = header.select { |column| column.match(/^.*-DIRECTION$/i) }.map(&:capitalize)
      @typographies = header.select { |column| column.match(/^.*-TYPOGRAPHY$/i) }.map(&:capitalize)
      @languages = header.select { |column| column.match(/^(\w{1,3})$/i ) }.map(&:capitalize)
    end

    # Check for undefined languages in language specific typographies
    # Any undefined language specific typographies are removed
    @typographies.select! do |typography|
      language, _ = typography.split('-')

      if not @languages.include?(language)
        raise "#{$warning_prefix} Unknown language in column #{language}-TYPOGRAPHY"
      end

      @languages.include?(language)
    end

    # Check for undefined languages in language specific alignments
    # Any undefined language specific alignments are removed
    @alignments.select! do |alignment|
      language, _ = alignment.split('-')

      if not @languages.include?(language)
        raise "#{$warning_prefix} Unknown language in column #{language}-ALIGNMENT"
      end

      @languages.include?(language)
    end

    # Check for undefined languages in language specific directions
    # Any undefined language specific directions are removed
    @directions.select! do |direction|
      language, _ = direction.split('-')

      if not @languages.include?(language)
        raise "#{$warning_prefix} Unknown language in column #{language}-DIRECTION"
      end

      @languages.include?(language)
    end

    reader.read_rows do |row|
      text_id = row[:"Text ID"]
      default_typography = row[:"Typography Name"]
      default_alignment = row[:Alignment]
      if row.exists?(:Direction)
        default_direction = row[:Direction]
      end

      text_id = text_id.strip if text_id
      default_typography = default_typography.strip if default_typography
      default_alignment = default_alignment.strip if default_alignment
      default_direction = default_direction.strip if default_direction

      if text_id && default_typography
        unless text_id.match(/^([0-9a-zA-Z_])*$/)
          puts "Illegal characters found in Text ID '#{text_id}'"
          fail
        end
        
        text_entry = TextEntry.new(text_id, default_typography, default_alignment, default_direction)

        @typographies.each do |typography|
          language, _ = typography.split('-')
          #puts "adding typography #{typography}"
          t = row[typography]
          t = t.strip if t
          text_entry.add_typography(language, t)
        end

        @alignments.each do |alignment|
          language, _ = alignment.split('-')
          #puts "adding alignment #{alignment}"
          a = row[alignment]
          a = a.strip if a
          text_entry.add_alignment(language, a)
        end

        @directions.each do |direction|
          language, _ = direction.split('-')
          #puts "adding direction #{direction}"
          d = row[direction]
          d = d.strip if d
          text_entry.add_direction(language, d)
        end

        @languages.each do |language|
          #puts "adding language #{language}"
          # Do *not* strip leading/trailing whitespace from translations.
          text_entry.add_translation(language, row[language])
        end

        @text_entries.add(text_entry)
      end
    end
    @text_entries
  end

end
