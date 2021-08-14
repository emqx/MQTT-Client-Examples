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

class Typography < Struct.new(:name, :font_file, :font_size, :bpp, :fallback_character, :ellipsis_character, :wildcard_characters, :wildcard_ranges)
  def cpp_name
    font_file.gsub(/\.ttf$/,"").gsub(".", "_").gsub(" ", "_").gsub("-","_")
  end
end
