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

require 'erb'
require 'lib/file_io'

class Template
  attr_accessor :text_entries
  attr_accessor :typographies

  def initialize(text_entries, typographies, output_directory)
    @text_entries = text_entries
    @typographies = typographies
    @output_directory = output_directory
  end
  def run
    result = ERB.new(File.read(input_path).
        gsub(WINDOWS_LINE_ENDINGS, UNIX_LINE_ENDINGS),0,"<>").result( binding ).
        gsub(WINDOWS_LINE_ENDINGS, UNIX_LINE_ENDINGS).
        gsub(UNIX_LINE_ENDINGS, LINE_ENDINGS)
    FileIO.write_file(File.join(@output_directory, output_path), result)
  end
end

