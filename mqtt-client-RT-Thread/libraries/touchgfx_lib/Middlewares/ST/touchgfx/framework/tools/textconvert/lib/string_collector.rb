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

CollectorStruct = Struct.new(:text_entries, :typographies)

$warning_prefix = "\nWARNING (TextConverter): "

class StringCollector < CollectorStruct
  def run
    string_indices = {}
    characters = Array.new

    #collect all strings for sorting
    all_strings = Array.new
    text_entries.each do |text_entry|
      text_entry.translations.each do |translation|
        all_strings.push(translation)
      end
    end
    #sort by length
    all_strings.sort_by!(){|x| -x.length}

    #collect all string indeces, and add to characters array
    all_strings.each do |translation|
      #lookup translation in hash
      #if not found, add to characters and insert index in hash for translation and all suffices
      #if found, do nothing
      unicodes = translation.unicodes
      index = string_indices[unicodes]
      if not index
        new_index = characters.length
        #puts "new string: #{translation.to_cpp} index: #{new_index}"
        characters.concat(unicodes).push(0)
        for start in 0 .. unicodes.length-1
          sub_string = unicodes[start..-1]
          # if the substring is present, all shorter substrings are also present, so do not add again
          break if string_indices[sub_string]
          string_indices[sub_string] = (new_index + start)
        end
      else
        #puts "existing string: #{translation.to_cpp} index: #{index}"
      end
    end
    #puts characters.inject("") {|t,i| "#{t}#{i==0?'|' : i.chr}"}
    #puts "Total: #{characters.length} chars"
    #puts string_indices
    [string_indices, characters]
  end
end
