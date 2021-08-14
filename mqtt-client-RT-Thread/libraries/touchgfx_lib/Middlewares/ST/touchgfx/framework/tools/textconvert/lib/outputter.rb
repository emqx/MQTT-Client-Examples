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

require 'lib/file_io'
require 'lib/emitters/template'
require 'lib/emitters/text_keys_and_languages_hpp'
require 'lib/emitters/texts_cpp'
require 'lib/emitters/languages_cpp'
require 'lib/emitters/languages_bin'
require 'lib/emitters/unicodes_txt'
require 'lib/emitters/fonts_cpp'
require 'lib/emitters/generated_font_cpp'
require 'lib/emitters/generated_font_hpp'
require 'lib/emitters/unmapped_data_font_cpp'
require 'lib/emitters/unmapped_data_font_hpp'
require 'lib/emitters/cached_font_cpp'
require 'lib/emitters/cached_font_hpp'
require 'lib/emitters/font_cache_cpp'
require 'lib/emitters/font_cache_hpp'
require 'lib/emitters/application_font_provider_hpp'
require 'lib/emitters/application_font_provider_cpp'
require 'lib/emitters/typed_text_database_hpp'
require 'lib/emitters/typed_text_database_cpp'

class Outputter
  def initialize(string_indices, characters, text_entries, typographies, localization_output_directory, fonts_output_directory, font_asset_path, data_format, remap_identical_texts, generate_binary_language_files, generate_binary_font_files, generate_font_format)
    @string_indices = string_indices #dictionary of all string indices into the characters array
    @characters = characters         #one array of the needed strings in optimal order
    @text_entries = text_entries
    @typographies = typographies
    @localization_output_directory = localization_output_directory
    @fonts_output_directory = fonts_output_directory
    @font_asset_path = font_asset_path
    @data_format = data_format
    @remap_identical_texts = remap_identical_texts
    @generate_binary_language_files = generate_binary_language_files
    @generate_binary_font_files = generate_binary_font_files
    @generate_font_format = generate_font_format
  end

  def run

    #puts "Running Outputter1, #{Time.now.strftime("%H:%M:%S:%L")}"
    [ GeneratedFontHpp,
      GeneratedFontCpp,
      UnmappedDataFontHpp,
      UnmappedDataFontCpp,
      CachedFontHpp,
      CachedFontCpp,
      FontCacheHpp,
      FontCacheCpp,
      UnicodesTxt ].each { |template| template.new(@text_entries, @typographies, @fonts_output_directory).run }

    [ ApplicationFontProviderCpp,
      ApplicationFontProviderHpp ].each { |template| template.new(@text_entries, @typographies, @fonts_output_directory, @generate_font_format).run }

    #puts "Running Outputter2, #{Time.now.strftime("%H:%M:%S:%L")}"
    [ TextKeysAndLanguages,
      TypedTextDatabaseHpp].each { |template| template.new(@text_entries, @typographies, @localization_output_directory).run }

    #puts "Running Outputter3, #{Time.now.strftime("%H:%M:%S:%L")}"
    TypedTextDatabaseCpp.new(@text_entries, @typographies, @localization_output_directory, @generate_binary_language_files, @generate_font_format).run

    #puts "Running Outputter4, #{Time.now.strftime("%H:%M:%S:%L")}"
    TextsCpp.new(@characters, @text_entries, @typographies, @localization_output_directory, @remap_identical_texts, @generate_binary_language_files).run

    #puts "Running Outputter5, #{Time.now.strftime("%H:%M:%S:%L")}"
    LanguagesCpp.new(@string_indices, @text_entries, @localization_output_directory, @remap_identical_texts, @generate_binary_language_files).run

    #puts "Running Outputter6, #{Time.now.strftime("%H:%M:%S:%L")}"
    FontsCpp.new(@text_entries, @typographies, @fonts_output_directory, @font_asset_path, @data_format, @generate_binary_font_files, @generate_font_format).run

    if @generate_binary_language_files.downcase == 'yes'
      #puts "Running Outputter7, #{Time.now.strftime("%H:%M:%S:%L")}"
      [ LanguagesBin ].each { |template| template.new(@text_entries, @typographies, @localization_output_directory).run }
    end
    #puts "Done Outputter, #{Time.now.strftime("%H:%M:%S:%L")}"
  end
end

