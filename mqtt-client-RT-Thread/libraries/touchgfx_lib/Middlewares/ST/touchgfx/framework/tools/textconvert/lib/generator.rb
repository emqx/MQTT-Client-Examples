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

require 'lib/text_entries_excel_reader'
require 'lib/typographies_excel_reader'
require 'lib/sanitizer'
require 'lib/string_collector'
require 'lib/outputter'

class Generator
  def run(file_name, output_path, text_output_path, font_asset_path, data_format, remap_identical_texts, generate_binary_language_files, generate_binary_font_files, framebuffer_bpp, generate_font_format)
    #puts "Running TextEntriesExcelReader, #{Time.now.strftime("%H:%M:%S:%L")}"
    text_entries = TextEntriesExcelReader.new(file_name).run
    #puts "Running TypoEntriesExcelReader, #{Time.now.strftime("%H:%M:%S:%L")}"
    typographies = TypographiesExcelReader.new(file_name).run
    #puts "Running Sanitizer, #{Time.now.strftime("%H:%M:%S:%L")}"
    Sanitizer.new(text_entries, typographies, framebuffer_bpp).run
    #puts "Running StringCollector, #{Time.now.strftime("%H:%M:%S:%L")}"
    if remap_identical_texts=='yes'
      string_indices, characters = StringCollector.new(text_entries, typographies).run
    end
    #puts "Running Outputter, #{Time.now.strftime("%H:%M:%S:%L")}"
    Outputter.new(string_indices, characters, text_entries, typographies, text_output_path, output_path, font_asset_path, data_format, remap_identical_texts, generate_binary_language_files, generate_binary_font_files, generate_font_format).run
  end
end
