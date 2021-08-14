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

class UnicodesTxt
  def initialize(text_entries, typographies, output_directory)
    @text_entries = text_entries
    @typographies = typographies
    @output_directory = output_directory
  end
  def run
    unique_typographies = @typographies.map{ |t| Typography.new("", t.font_file, t.font_size, t.bpp) }.uniq.sort_by { |t| sprintf("%s %04d %d",t.font_file,t.font_size,t.bpp) }
    unique_typographies.each do |unique_typography|
      UnicodeForTypographyTxt.new(@text_entries, @output_directory, @typographies, unique_typography).run
    end
  end
end

class UnicodeForTypographyTxt
  def initialize(text_entries, output_directory, typographies, unique_typography)
    @text_entries = text_entries
    @output_directory = output_directory
    @typographies = typographies
    @unique_typography = unique_typography
  end

  def convert_to_contextual_forms(unicodes)
    unicodes.sort!
    [
      [[0x0621],[0xFE80]], # ARABIC LETTER HAMZA
      [[0x0622],[0xFE81,0xFE82]], # ARABIC LETTER ALEF WITH MADDA ABOVE
      [[0x0622,0x0644],[0xFEF5,0xFEF6]], # ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE
      [[0x0623],[0xFE83,0xFE84]], # ARABIC LETTER ALEF WITH HAMZA ABOVE
      [[0x0623,0x0644],[0xFEF7,0xFEF8]], # ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE
      [[0x0624],[0xFE85,0xFE86]], # ARABIC LETTER WAW WITH HAMZA ABOVE
      [[0x0625],[0xFE87,0xFE88]], # ARABIC LETTER ALEF WITH HAMZA BELOW
      [[0x0625,0x0644],[0xFEF9,0xFEFA]], # ARABIC LIGATURE LAM WITH ALEF WITH HAMZA BELOW
      [[0x0626],[0xFE89,0xFE8A,0xFE8B,0xFE8C]], # ARABIC LETTER YEH WITH HAMZA ABOVE
      [[0x0626,0x0627],[0xFBEA,0xFBEB]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH ALEF
      [[0x0626,0x062C],[0xFC00,0xFC97]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH JEEM
      [[0x0626,0x062D],[0xFC01,0xFC98]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH HAH
      [[0x0626,0x062E],[0xFC99]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH KHAH
      [[0x0626,0x0631],[0xFC64]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH REH
      [[0x0626,0x0632],[0xFC65]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH ZAIN
      [[0x0626,0x0645],[0xFC02,0xFC66,0xFC9A,0xFCDF]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH MEEM
      [[0x0626,0x0646],[0xFC67]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH NOON
      [[0x0626,0x0647],[0xFC9B,0xFCE0]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH HEH
      [[0x0626,0x0648],[0xFBEE,0xFBEF]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH WAW
      [[0x0626,0x0649],[0xFBF9,0xFBFA,0xFBFB,0xFC03,0xFC68]], # ARABIC LIGATURE UIGHUR KIRGHIZ YEH WITH HAMZA ABOVE WITH ALEF MAKSURA / ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH ALEF MAKSURA
      [[0x0626,0x064A],[0xFC04,0xFC69]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH YEH
      [[0x0626,0x06C6],[0xFBF2,0xFBF3]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH OE
      [[0x0626,0x06C7],[0xFBF0,0xFBF1]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH U
      [[0x0626,0x06C8],[0xFBF4,0xFBF5]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH YU
      [[0x0626,0x06D0],[0xFBF6,0xFBF7,0xFBF8]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH E
      [[0x0626,0x06D5],[0xFBEC,0xFBED]], # ARABIC LIGATURE YEH WITH HAMZA ABOVE WITH AE
      [[0x0627],[0xFE8D,0xFE8E]], # ARABIC LETTER ALEF
      [[0x0627,0x0628,0x0631,0x0643],[0xFDF3]], # ARABIC LIGATURE AKBAR
      [[0x0627,0x0631,0x0644,0x06CC],[0xFDFC]], # RIAL SIGN
      [[0x0627,0x0643],[0xFC37,0xFC80]], # ARABIC LIGATURE KAF WITH ALEF
      [[0x0627,0x0644],[0xFEFB,0xFEFC]], # ARABIC LIGATURE LAM WITH ALEF
      [[0x0627,0x0644,0x0647],[0xFDF2]], # ARABIC LIGATURE ALLAH
      [[0x0627,0x0645],[0xFC88]], # ARABIC LIGATURE MEEM WITH ALEF
      [[0x0627,0x064B],[0xFD3C,0xFD3D]], # ARABIC LIGATURE ALEF WITH FATHATAN
      [[0x0628],[0xFE8F,0xFE90,0xFE91,0xFE92]], # ARABIC LETTER BEH
      [[0x0628,0x062C],[0xFC05,0xFC9C]], # ARABIC LIGATURE BEH WITH JEEM
      [[0x0628,0x062D],[0xFC06,0xFC9D]], # ARABIC LIGATURE BEH WITH HAH
      [[0x0628,0x062D,0x064A],[0xFDC2]], # ARABIC LIGATURE BEH WITH HAH WITH YEH
      [[0x0628,0x062E],[0xFC07,0xFC9E]], # ARABIC LIGATURE BEH WITH KHAH
      [[0x0628,0x062E,0x064A],[0xFD9E]], # ARABIC LIGATURE BEH WITH KHAH WITH YEH
      [[0x0628,0x0631],[0xFC6A]], # ARABIC LIGATURE BEH WITH REH
      [[0x0628,0x0632],[0xFC6B]], # ARABIC LIGATURE BEH WITH ZAIN
      [[0x0628,0x0645],[0xFC08,0xFC6C,0xFC9F,0xFCE1]], # ARABIC LIGATURE BEH WITH MEEM
      [[0x0628,0x0646],[0xFC6D]], # ARABIC LIGATURE BEH WITH NOON
      [[0x0628,0x0647],[0xFCA0,0xFCE2]], # ARABIC LIGATURE BEH WITH HEH
      [[0x0628,0x0649],[0xFC09,0xFC6E]], # ARABIC LIGATURE BEH WITH ALEF MAKSURA
      [[0x0628,0x064A],[0xFC0A,0xFC6F]], # ARABIC LIGATURE BEH WITH YEH
      [[0x0629],[0xFE93,0xFE94]], # ARABIC LETTER TEH MARBUTA
      [[0x062A],[0xFE95,0xFE96,0xFE97,0xFE98]], # ARABIC LETTER TEH
      [[0x062A,0x062C],[0xFC0B,0xFCA1]], # ARABIC LIGATURE TEH WITH JEEM
      [[0x062A,0x062C,0x062D],[0xFD51,0xFD52]], # ARABIC LIGATURE TEH WITH HAH WITH JEEM
      [[0x062A,0x062C,0x0645],[0xFD50,0xFD55]], # ARABIC LIGATURE TEH WITH JEEM WITH MEEM / ARABIC LIGATURE TEH WITH MEEM WITH JEEM
      [[0x062A,0x062C,0x0649],[0xFDA0]], # ARABIC LIGATURE TEH WITH JEEM WITH ALEF MAKSURA
      [[0x062A,0x062C,0x064A],[0xFD9F]], # ARABIC LIGATURE TEH WITH JEEM WITH YEH
      [[0x062A,0x062D],[0xFC0C,0xFCA2]], # ARABIC LIGATURE TEH WITH HAH
      [[0x062A,0x062D,0x0645],[0xFD53,0xFD56]], # ARABIC LIGATURE TEH WITH HAH WITH MEEM / ARABIC LIGATURE TEH WITH MEEM WITH HAH
      [[0x062A,0x062E],[0xFC0D,0xFCA3]], # ARABIC LIGATURE TEH WITH KHAH
      [[0x062A,0x062E,0x0645],[0xFD54,0xFD57]], # ARABIC LIGATURE TEH WITH KHAH WITH MEEM / ARABIC LIGATURE TEH WITH MEEM WITH KHAH
      [[0x062A,0x062E,0x0649],[0xFDA2]], # ARABIC LIGATURE TEH WITH KHAH WITH ALEF MAKSURA
      [[0x062A,0x062E,0x064A],[0xFDA1]], # ARABIC LIGATURE TEH WITH KHAH WITH YEH
      [[0x062A,0x0631],[0xFC70]], # ARABIC LIGATURE TEH WITH REH
      [[0x062A,0x0632],[0xFC71]], # ARABIC LIGATURE TEH WITH ZAIN
      [[0x062A,0x0645],[0xFC0E,0xFC72,0xFCA4,0xFCE3]], # ARABIC LIGATURE TEH WITH MEEM
      [[0x062A,0x0645,0x0649],[0xFDA4]], # ARABIC LIGATURE TEH WITH MEEM WITH ALEF MAKSURA
      [[0x062A,0x0645,0x064A],[0xFDA3]], # ARABIC LIGATURE TEH WITH MEEM WITH YEH
      [[0x062A,0x0646],[0xFC73]], # ARABIC LIGATURE TEH WITH NOON
      [[0x062A,0x0647],[0xFCA5,0xFCE4]], # ARABIC LIGATURE TEH WITH HEH
      [[0x062A,0x0649],[0xFC0F,0xFC74]], # ARABIC LIGATURE TEH WITH ALEF MAKSURA
      [[0x062A,0x064A],[0xFC10,0xFC75]], # ARABIC LIGATURE TEH WITH YEH
      [[0x062B],[0xFE99,0xFE9A,0xFE9B,0xFE9C]], # ARABIC LETTER THEH
      [[0x062B,0x062C],[0xFC11]], # ARABIC LIGATURE THEH WITH JEEM
      [[0x062B,0x0631],[0xFC76]], # ARABIC LIGATURE THEH WITH REH
      [[0x062B,0x0632],[0xFC77]], # ARABIC LIGATURE THEH WITH ZAIN
      [[0x062B,0x0645],[0xFC12,0xFC78,0xFCA6,0xFCE5]], # ARABIC LIGATURE THEH WITH MEEM
      [[0x062B,0x0646],[0xFC79]], # ARABIC LIGATURE THEH WITH NOON
      [[0x062B,0x0647],[0xFCE6]], # ARABIC LIGATURE THEH WITH HEH
      [[0x062B,0x0649],[0xFC13,0xFC7A]], # ARABIC LIGATURE THEH WITH ALEF MAKSURA
      [[0x062B,0x064A],[0xFC14,0xFC7B]], # ARABIC LIGATURE THEH WITH YEH
      [[0x062C],[0xFE9D,0xFE9E,0xFE9F,0xFEA0]], # ARABIC LETTER JEEM
      [[0x062C,0x062D],[0xFC15,0xFC17,0xFCA7,0xFCA9]], # ARABIC LIGATURE JEEM WITH HAH / ARABIC LIGATURE HAH WITH JEEM
      [[0x062C,0x062D,0x0633],[0xFD5C,0xFD5D]], # ARABIC LIGATURE SEEN WITH HAH WITH JEEM / ARABIC LIGATURE SEEN WITH JEEM WITH HAH
      [[0x062C,0x062D,0x0645],[0xFD58,0xFD59,0xFD89,0xFD8C]], # ARABIC LIGATURE JEEM WITH MEEM WITH HAH / ARABIC LIGATURE MEEM WITH HAH WITH JEEM / ARABIC LIGATURE MEEM WITH JEEM WITH HAH
      [[0x062C,0x062D,0x0646],[0xFDB8,0xFDBD]], # ARABIC LIGATURE NOON WITH JEEM WITH HAH
      [[0x062C,0x062D,0x0649],[0xFDA6]], # ARABIC LIGATURE JEEM WITH HAH WITH ALEF MAKSURA
      [[0x062C,0x062D,0x064A],[0xFDBE,0xFDBF]], # ARABIC LIGATURE JEEM WITH HAH WITH YEH / ARABIC LIGATURE HAH WITH JEEM WITH YEH
      [[0x062C,0x062E],[0xFC19,0xFCAB]], # ARABIC LIGATURE KHAH WITH JEEM
      [[0x062C,0x062E,0x0645],[0xFD8E,0xFD92]], # ARABIC LIGATURE MEEM WITH KHAH WITH JEEM / ARABIC LIGATURE MEEM WITH JEEM WITH KHAH
      [[0x062C,0x0633],[0xFC1C,0xFCAD,0xFD34]], # ARABIC LIGATURE SEEN WITH JEEM
      [[0x062C,0x0633,0x0645],[0xFD61]], # ARABIC LIGATURE SEEN WITH MEEM WITH JEEM
      [[0x062C,0x0633,0x0649],[0xFD5E]], # ARABIC LIGATURE SEEN WITH JEEM WITH ALEF MAKSURA
      [[0x062C,0x0634],[0xFD09,0xFD25,0xFD2D,0xFD37]], # ARABIC LIGATURE SHEEN WITH JEEM
      [[0x062C,0x0634,0x064A],[0xFD69]], # ARABIC LIGATURE SHEEN WITH JEEM WITH YEH
      [[0x062C,0x0636],[0xFC22,0xFCB4]], # ARABIC LIGATURE DAD WITH JEEM
      [[0x062C,0x0639],[0xFC29,0xFCBA]], # ARABIC LIGATURE AIN WITH JEEM
      [[0x062C,0x0639,0x0645],[0xFD75,0xFDC4]], # ARABIC LIGATURE AIN WITH JEEM WITH MEEM
      [[0x062C,0x063A],[0xFC2B,0xFCBC]], # ARABIC LIGATURE GHAIN WITH JEEM
      [[0x062C,0x0641],[0xFC2D,0xFCBE]], # ARABIC LIGATURE FEH WITH JEEM
      [[0x062C,0x0643],[0xFC38,0xFCC4]], # ARABIC LIGATURE KAF WITH JEEM
      [[0x062C,0x0644],[0xFC3F,0xFCC9,0xFD83,0xFD84]], # ARABIC LIGATURE LAM WITH JEEM / ARABIC LIGATURE LAM WITH JEEM WITH JEEM
      [[0x062C,0x0644,0x0645],[0xFDBA,0xFDBC]], # ARABIC LIGATURE LAM WITH JEEM WITH MEEM
      [[0x062C,0x0644,0x064A],[0xFDAC]], # ARABIC LIGATURE LAM WITH JEEM WITH YEH
      [[0x062C,0x0645],[0xFC16,0xFC45,0xFCA8,0xFCCE,0xFD8D]], # ARABIC LIGATURE JEEM WITH MEEM / ARABIC LIGATURE MEEM WITH JEEM / ARABIC LIGATURE MEEM WITH JEEM WITH MEEM
      [[0x062C,0x0645,0x0646],[0xFD97,0xFD98]], # ARABIC LIGATURE NOON WITH JEEM WITH MEEM
      [[0x062C,0x0645,0x0647],[0xFD93]], # ARABIC LIGATURE HEH WITH MEEM WITH JEEM
      [[0x062C,0x0645,0x0649],[0xFDA7]], # ARABIC LIGATURE JEEM WITH MEEM WITH ALEF MAKSURA
      [[0x062C,0x0645,0x064A],[0xFDA5,0xFDC0]], # ARABIC LIGATURE JEEM WITH MEEM WITH YEH / ARABIC LIGATURE MEEM WITH JEEM WITH YEH
      [[0x062C,0x0646],[0xFC4B,0xFCD2]], # ARABIC LIGATURE NOON WITH JEEM
      [[0x062C,0x0646,0x0649],[0xFD99]], # ARABIC LIGATURE NOON WITH JEEM WITH ALEF MAKSURA
      [[0x062C,0x0646,0x064A],[0xFDC7]], # ARABIC LIGATURE NOON WITH JEEM WITH YEH
      [[0x062C,0x0647],[0xFC51,0xFCD7]], # ARABIC LIGATURE HEH WITH JEEM
      [[0x062C,0x0649],[0xFD01,0xFD1D]], # ARABIC LIGATURE JEEM WITH ALEF MAKSURA
      [[0x062C,0x064A],[0xFC55,0xFCDA,0xFD02,0xFD1E,0xFDAF]], # ARABIC LIGATURE YEH WITH JEEM / ARABIC LIGATURE JEEM WITH YEH / ARABIC LIGATURE YEH WITH JEEM WITH YEH
      [[0x062D],[0xFEA1,0xFEA2,0xFEA3,0xFEA4]], # ARABIC LETTER HAH
      [[0x062D,0x062E],[0xFC1A]], # ARABIC LIGATURE KHAH WITH HAH
      [[0x062D,0x062F,0x0645],[0xFDF4]], # ARABIC LIGATURE MOHAMMAD
      [[0x062D,0x0633],[0xFC1D,0xFCAE,0xFD35]], # ARABIC LIGATURE SEEN WITH HAH
      [[0x062D,0x0633,0x0645],[0xFD5F,0xFD60]], # ARABIC LIGATURE SEEN WITH MEEM WITH HAH
      [[0x062D,0x0634],[0xFD0A,0xFD26,0xFD2E,0xFD38]], # ARABIC LIGATURE SHEEN WITH HAH
      [[0x062D,0x0634,0x0645],[0xFD67,0xFD68]], # ARABIC LIGATURE SHEEN WITH HAH WITH MEEM
      [[0x062D,0x0634,0x064A],[0xFDAA]], # ARABIC LIGATURE SHEEN WITH HAH WITH YEH
      [[0x062D,0x0635],[0xFC20,0xFCB1,0xFD64,0xFD65]], # ARABIC LIGATURE SAD WITH HAH / ARABIC LIGATURE SAD WITH HAH WITH HAH
      [[0x062D,0x0635,0x064A],[0xFDA9]], # ARABIC LIGATURE SAD WITH HAH WITH YEH
      [[0x062D,0x0636],[0xFC23,0xFCB5]], # ARABIC LIGATURE DAD WITH HAH
      [[0x062D,0x0636,0x0649],[0xFD6E]], # ARABIC LIGATURE DAD WITH HAH WITH ALEF MAKSURA
      [[0x062D,0x0636,0x064A],[0xFDAB]], # ARABIC LIGATURE DAD WITH HAH WITH YEH
      [[0x062D,0x0637],[0xFC26,0xFCB8]], # ARABIC LIGATURE TAH WITH HAH
      [[0x062D,0x0637,0x0645],[0xFD71,0xFD72]], # ARABIC LIGATURE TAH WITH MEEM WITH HAH
      [[0x062D,0x0641],[0xFC2E,0xFCBF]], # ARABIC LIGATURE FEH WITH HAH
      [[0x062D,0x0642],[0xFC33,0xFCC2]], # ARABIC LIGATURE QAF WITH HAH
      [[0x062D,0x0642,0x0645],[0xFD7E,0xFDB4]], # ARABIC LIGATURE QAF WITH MEEM WITH HAH
      [[0x062D,0x0643],[0xFC39,0xFCC5]], # ARABIC LIGATURE KAF WITH HAH
      [[0x062D,0x0644],[0xFC40,0xFCCA]], # ARABIC LIGATURE LAM WITH HAH
      [[0x062D,0x0644,0x0645],[0xFD80,0xFD87,0xFD88,0xFDB5]], # ARABIC LIGATURE LAM WITH HAH WITH MEEM / ARABIC LIGATURE LAM WITH MEEM WITH HAH
      [[0x062D,0x0644,0x0649],[0xFD82]], # ARABIC LIGATURE LAM WITH HAH WITH ALEF MAKSURA
      [[0x062D,0x0644,0x064A],[0xFD81]], # ARABIC LIGATURE LAM WITH HAH WITH YEH
      [[0x062D,0x0645],[0xFC18,0xFC46,0xFCAA,0xFCCF,0xFD8A]], # ARABIC LIGATURE HAH WITH MEEM / ARABIC LIGATURE MEEM WITH HAH / ARABIC LIGATURE MEEM WITH HAH WITH MEEM
      [[0x062D,0x0645,0x0646],[0xFD95]], # ARABIC LIGATURE NOON WITH HAH WITH MEEM
      [[0x062D,0x0645,0x0649],[0xFD5B]], # ARABIC LIGATURE HAH WITH MEEM WITH ALEF MAKSURA
      [[0x062D,0x0645,0x064A],[0xFD5A,0xFD8B]], # ARABIC LIGATURE HAH WITH MEEM WITH YEH / ARABIC LIGATURE MEEM WITH HAH WITH YEH
      [[0x062D,0x0646],[0xFC4C,0xFCD3]], # ARABIC LIGATURE NOON WITH HAH
      [[0x062D,0x0646,0x0649],[0xFD96]], # ARABIC LIGATURE NOON WITH HAH WITH ALEF MAKSURA
      [[0x062D,0x0646,0x064A],[0xFDB3]], # ARABIC LIGATURE NOON WITH HAH WITH YEH
      [[0x062D,0x0649],[0xFCFF,0xFD1B]], # ARABIC LIGATURE HAH WITH ALEF MAKSURA
      [[0x062D,0x064A],[0xFC56,0xFCDB,0xFD00,0xFD1C,0xFDAE]], # ARABIC LIGATURE YEH WITH HAH / ARABIC LIGATURE HAH WITH YEH / ARABIC LIGATURE YEH WITH HAH WITH YEH
      [[0x062E],[0xFEA5,0xFEA6,0xFEA7,0xFEA8]], # ARABIC LETTER KHAH
      [[0x062E,0x0633],[0xFC1E,0xFCAF,0xFD36]], # ARABIC LIGATURE SEEN WITH KHAH
      [[0x062E,0x0633,0x0649],[0xFDA8]], # ARABIC LIGATURE SEEN WITH KHAH WITH ALEF MAKSURA
      [[0x062E,0x0633,0x064A],[0xFDC6]], # ARABIC LIGATURE SEEN WITH KHAH WITH YEH
      [[0x062E,0x0634],[0xFD0B,0xFD27,0xFD2F,0xFD39]], # ARABIC LIGATURE SHEEN WITH KHAH
      [[0x062E,0x0634,0x0645],[0xFD6A,0xFD6B]], # ARABIC LIGATURE SHEEN WITH MEEM WITH KHAH
      [[0x062E,0x0635],[0xFCB2]], # ARABIC LIGATURE SAD WITH KHAH
      [[0x062E,0x0636],[0xFC24,0xFCB6]], # ARABIC LIGATURE DAD WITH KHAH
      [[0x062E,0x0636,0x0645],[0xFD6F,0xFD70]], # ARABIC LIGATURE DAD WITH KHAH WITH MEEM
      [[0x062E,0x0641],[0xFC2F,0xFCC0]], # ARABIC LIGATURE FEH WITH KHAH
      [[0x062E,0x0641,0x0645],[0xFD7C,0xFD7D]], # ARABIC LIGATURE FEH WITH KHAH WITH MEEM
      [[0x062E,0x0643],[0xFC3A,0xFCC6]], # ARABIC LIGATURE KAF WITH KHAH
      [[0x062E,0x0644],[0xFC41,0xFCCB]], # ARABIC LIGATURE LAM WITH KHAH
      [[0x062E,0x0644,0x0645],[0xFD85,0xFD86]], # ARABIC LIGATURE LAM WITH KHAH WITH MEEM
      [[0x062E,0x0645],[0xFC1B,0xFC47,0xFCAC,0xFCD0,0xFD8F]], # ARABIC LIGATURE KHAH WITH MEEM / ARABIC LIGATURE MEEM WITH KHAH / ARABIC LIGATURE MEEM WITH KHAH WITH MEEM
      [[0x062E,0x0645,0x064A],[0xFDB9]], # ARABIC LIGATURE MEEM WITH KHAH WITH YEH
      [[0x062E,0x0646],[0xFC4D,0xFCD4]], # ARABIC LIGATURE NOON WITH KHAH
      [[0x062E,0x0649],[0xFD03,0xFD1F]], # ARABIC LIGATURE KHAH WITH ALEF MAKSURA
      [[0x062E,0x064A],[0xFC57,0xFCDC,0xFD04,0xFD20]], # ARABIC LIGATURE YEH WITH KHAH / ARABIC LIGATURE KHAH WITH YEH
      [[0x062F],[0xFEA9,0xFEAA]], # ARABIC LETTER DAL
      [[0x0630],[0xFEAB,0xFEAC]], # ARABIC LETTER THAL
      [[0x0630,0x0670],[0xFC5B]], # ARABIC LIGATURE THAL WITH SUPERSCRIPT ALEF
      [[0x0631],[0xFEAD,0xFEAE]], # ARABIC LETTER REH
      [[0x0631,0x0633],[0xFD0E,0xFD2A]], # ARABIC LIGATURE SEEN WITH REH
      [[0x0631,0x0633,0x0644,0x0648],[0xFDF6]], # ARABIC LIGATURE RASOUL
      [[0x0631,0x0634],[0xFD0D,0xFD29]], # ARABIC LIGATURE SHEEN WITH REH
      [[0x0631,0x0635],[0xFD0F,0xFD2B]], # ARABIC LIGATURE SAD WITH REH
      [[0x0631,0x0636],[0xFD10,0xFD2C]], # ARABIC LIGATURE DAD WITH REH
      [[0x0631,0x0646],[0xFC8A]], # ARABIC LIGATURE NOON WITH REH
      [[0x0631,0x064A],[0xFC91]], # ARABIC LIGATURE YEH WITH REH
      [[0x0631,0x0670],[0xFC5C]], # ARABIC LIGATURE REH WITH SUPERSCRIPT ALEF
      [[0x0632],[0xFEAF,0xFEB0]], # ARABIC LETTER ZAIN
      [[0x0632,0x0646],[0xFC8B]], # ARABIC LIGATURE NOON WITH ZAIN
      [[0x0632,0x064A],[0xFC92]], # ARABIC LIGATURE YEH WITH ZAIN
      [[0x0633],[0xFEB1,0xFEB2,0xFEB3,0xFEB4]], # ARABIC LETTER SEEN
      [[0x0633,0x0644,0x0645,0x0648],[0xFDF8]], # ARABIC LIGATURE WASALLAM
      [[0x0633,0x0645],[0xFC1F,0xFCB0,0xFCE7,0xFD62,0xFD63]], # ARABIC LIGATURE SEEN WITH MEEM / ARABIC LIGATURE SEEN WITH MEEM WITH MEEM
      [[0x0633,0x0647],[0xFCE8,0xFD31]], # ARABIC LIGATURE SEEN WITH HEH
      [[0x0633,0x0649],[0xFCFB,0xFD17]], # ARABIC LIGATURE SEEN WITH ALEF MAKSURA
      [[0x0633,0x064A],[0xFCFC,0xFD18]], # ARABIC LIGATURE SEEN WITH YEH
      [[0x0634],[0xFEB5,0xFEB6,0xFEB7,0xFEB8]], # ARABIC LETTER SHEEN
      [[0x0634,0x0645],[0xFCE9,0xFD0C,0xFD28,0xFD30,0xFD6C,0xFD6D]], # ARABIC LIGATURE SHEEN WITH MEEM / ARABIC LIGATURE SHEEN WITH MEEM WITH MEEM
      [[0x0634,0x0647],[0xFCEA,0xFD32]], # ARABIC LIGATURE SHEEN WITH HEH
      [[0x0634,0x0649],[0xFCFD,0xFD19]], # ARABIC LIGATURE SHEEN WITH ALEF MAKSURA
      [[0x0634,0x064A],[0xFCFE,0xFD1A]], # ARABIC LIGATURE SHEEN WITH YEH
      [[0x0635],[0xFEB9,0xFEBA,0xFEBB,0xFEBC]], # ARABIC LETTER SAD
      [[0x0635,0x0639,0x0644,0x0645],[0xFDF5]], # ARABIC LIGATURE SALAM
      [[0x0635,0x0644,0x0649],[0xFDF9]], # ARABIC LIGATURE SALLA
      [[0x0635,0x0644,0x06D2],[0xFDF0]], # ARABIC LIGATURE SALLA USED AS KORANIC STOP SIGN
      [[0x0635,0x0645],[0xFC21,0xFCB3,0xFD66,0xFDC5]], # ARABIC LIGATURE SAD WITH MEEM / ARABIC LIGATURE SAD WITH MEEM WITH MEEM
      [[0x0635,0x0649],[0xFD05,0xFD21]], # ARABIC LIGATURE SAD WITH ALEF MAKSURA
      [[0x0635,0x064A],[0xFD06,0xFD22]], # ARABIC LIGATURE SAD WITH YEH
      [[0x0636],[0xFEBD,0xFEBE,0xFEBF,0xFEC0]], # ARABIC LETTER DAD
      [[0x0636,0x0645],[0xFC25,0xFCB7]], # ARABIC LIGATURE DAD WITH MEEM
      [[0x0636,0x0649],[0xFD07,0xFD23]], # ARABIC LIGATURE DAD WITH ALEF MAKSURA
      [[0x0636,0x064A],[0xFD08,0xFD24]], # ARABIC LIGATURE DAD WITH YEH
      [[0x0637],[0xFEC1,0xFEC2,0xFEC3,0xFEC4]], # ARABIC LETTER TAH
      [[0x0637,0x0645],[0xFC27,0xFD33,0xFD3A,0xFD73]], # ARABIC LIGATURE TAH WITH MEEM / ARABIC LIGATURE TAH WITH MEEM WITH MEEM
      [[0x0637,0x0645,0x064A],[0xFD74]], # ARABIC LIGATURE TAH WITH MEEM WITH YEH
      [[0x0637,0x0649],[0xFCF5,0xFD11]], # ARABIC LIGATURE TAH WITH ALEF MAKSURA
      [[0x0637,0x064A],[0xFCF6,0xFD12]], # ARABIC LIGATURE TAH WITH YEH
      [[0x0638],[0xFEC5,0xFEC6,0xFEC7,0xFEC8]], # ARABIC LETTER ZAH
      [[0x0638,0x0645],[0xFC28,0xFCB9,0xFD3B]], # ARABIC LIGATURE ZAH WITH MEEM
      [[0x0639],[0xFEC9,0xFECA,0xFECB,0xFECC]], # ARABIC LETTER AIN
      [[0x0639,0x0644,0x0647,0x064A],[0xFDF7]], # ARABIC LIGATURE ALAYHE
      [[0x0639,0x0645],[0xFC2A,0xFCBB,0xFD76,0xFD77]], # ARABIC LIGATURE AIN WITH MEEM / ARABIC LIGATURE AIN WITH MEEM WITH MEEM
      [[0x0639,0x0645,0x0649],[0xFD78]], # ARABIC LIGATURE AIN WITH MEEM WITH ALEF MAKSURA
      [[0x0639,0x0645,0x064A],[0xFDB6]], # ARABIC LIGATURE AIN WITH MEEM WITH YEH
      [[0x0639,0x0649],[0xFCF7,0xFD13]], # ARABIC LIGATURE AIN WITH ALEF MAKSURA
      [[0x0639,0x064A],[0xFCF8,0xFD14]], # ARABIC LIGATURE AIN WITH YEH
      [[0x063A],[0xFECD,0xFECE,0xFECF,0xFED0]], # ARABIC LETTER GHAIN
      [[0x063A,0x0645],[0xFC2C,0xFCBD,0xFD79]], # ARABIC LIGATURE GHAIN WITH MEEM / ARABIC LIGATURE GHAIN WITH MEEM WITH MEEM
      [[0x063A,0x0645,0x0649],[0xFD7B]], # ARABIC LIGATURE GHAIN WITH MEEM WITH ALEF MAKSURA
      [[0x063A,0x0645,0x064A],[0xFD7A]], # ARABIC LIGATURE GHAIN WITH MEEM WITH YEH
      [[0x063A,0x0649],[0xFCF9,0xFD15]], # ARABIC LIGATURE GHAIN WITH ALEF MAKSURA
      [[0x063A,0x064A],[0xFCFA,0xFD16]], # ARABIC LIGATURE GHAIN WITH YEH
      [[0x0640,0x064B],[0xFE71]], # ARABIC TATWEEL WITH FATHATAN ABOVE
      [[0x0640,0x064E],[0xFE77]], # ARABIC FATHA
      [[0x0640,0x064E,0x0651],[0xFCF2]], # ARABIC LIGATURE SHADDA WITH FATHA
      [[0x0640,0x064F],[0xFE79]], # ARABIC DAMMA
      [[0x0640,0x064F,0x0651],[0xFCF3]], # ARABIC LIGATURE SHADDA WITH DAMMA
      [[0x0640,0x0650],[0xFE7B]], # ARABIC KASRA
      [[0x0640,0x0650,0x0651],[0xFCF4]], # ARABIC LIGATURE SHADDA WITH KASRA
      [[0x0640,0x0651],[0xFE7D]], # ARABIC SHADDA
      [[0x0640,0x0652],[0xFE7F]], # ARABIC SUKUN
      [[0x0641],[0xFED1,0xFED2,0xFED3,0xFED4]], # ARABIC LETTER FEH
      [[0x0641,0x0645],[0xFC30,0xFCC1]], # ARABIC LIGATURE FEH WITH MEEM
      [[0x0641,0x0645,0x064A],[0xFDC1]], # ARABIC LIGATURE FEH WITH MEEM WITH YEH
      [[0x0641,0x0649],[0xFC31,0xFC7C]], # ARABIC LIGATURE FEH WITH ALEF MAKSURA
      [[0x0641,0x064A],[0xFC32,0xFC7D]], # ARABIC LIGATURE FEH WITH YEH
      [[0x0642],[0xFED5,0xFED6,0xFED7,0xFED8]], # ARABIC LETTER QAF
      [[0x0642,0x0644,0x06D2],[0xFDF1]], # ARABIC LIGATURE QALA USED AS KORANIC STOP SIGN
      [[0x0642,0x0645],[0xFC34,0xFCC3,0xFD7F]], # ARABIC LIGATURE QAF WITH MEEM / ARABIC LIGATURE QAF WITH MEEM WITH MEEM
      [[0x0642,0x0645,0x064A],[0xFDB2]], # ARABIC LIGATURE QAF WITH MEEM WITH YEH
      [[0x0642,0x0649],[0xFC35,0xFC7E]], # ARABIC LIGATURE QAF WITH ALEF MAKSURA
      [[0x0642,0x064A],[0xFC36,0xFC7F]], # ARABIC LIGATURE QAF WITH YEH
      [[0x0643],[0xFED9,0xFEDA,0xFEDB,0xFEDC]], # ARABIC LETTER KAF
      [[0x0643,0x0644],[0xFC3B,0xFC81,0xFCC7,0xFCEB]], # ARABIC LIGATURE KAF WITH LAM
      [[0x0643,0x0645],[0xFC3C,0xFC82,0xFCC8,0xFCEC,0xFDBB,0xFDC3]], # ARABIC LIGATURE KAF WITH MEEM / ARABIC LIGATURE KAF WITH MEEM WITH MEEM
      [[0x0643,0x0645,0x064A],[0xFDB7]], # ARABIC LIGATURE KAF WITH MEEM WITH YEH
      [[0x0643,0x0649],[0xFC3D,0xFC83]], # ARABIC LIGATURE KAF WITH ALEF MAKSURA
      [[0x0643,0x064A],[0xFC3E,0xFC84]], # ARABIC LIGATURE KAF WITH YEH
      [[0x0644],[0xFEDD,0xFEDE,0xFEDF,0xFEE0]], # ARABIC LETTER LAM
      [[0x0644,0x0645],[0xFC42,0xFC85,0xFCCC,0xFCED]], # ARABIC LIGATURE LAM WITH MEEM
      [[0x0644,0x0645,0x064A],[0xFDAD]], # ARABIC LIGATURE LAM WITH MEEM WITH YEH
      [[0x0644,0x0647],[0xFCCD]], # ARABIC LIGATURE LAM WITH HEH
      [[0x0644,0x0649],[0xFC43,0xFC86]], # ARABIC LIGATURE LAM WITH ALEF MAKSURA
      [[0x0644,0x064A],[0xFC44,0xFC87]], # ARABIC LIGATURE LAM WITH YEH
      [[0x0645],[0xFC48,0xFC89,0xFCD1,0xFEE1,0xFEE2,0xFEE3,0xFEE4]], # ARABIC LIGATURE MEEM WITH MEEM / ARABIC LETTER MEEM
      [[0x0645,0x0646],[0xFC4E,0xFC8C,0xFCD5,0xFCEE]], # ARABIC LIGATURE NOON WITH MEEM
      [[0x0645,0x0646,0x0649],[0xFD9B]], # ARABIC LIGATURE NOON WITH MEEM WITH ALEF MAKSURA
      [[0x0645,0x0646,0x064A],[0xFD9A]], # ARABIC LIGATURE NOON WITH MEEM WITH YEH
      [[0x0645,0x0647],[0xFC52,0xFCD8,0xFD94]], # ARABIC LIGATURE HEH WITH MEEM / ARABIC LIGATURE HEH WITH MEEM WITH MEEM
      [[0x0645,0x0649],[0xFC49]], # ARABIC LIGATURE MEEM WITH ALEF MAKSURA
      [[0x0645,0x064A],[0xFC4A,0xFC58,0xFC93,0xFCDD,0xFCF0,0xFD9C,0xFD9D,0xFDB0,0xFDB1]], # ARABIC LIGATURE MEEM WITH YEH / ARABIC LIGATURE YEH WITH MEEM / ARABIC LIGATURE YEH WITH MEEM WITH MEEM / ARABIC LIGATURE YEH WITH MEEM WITH YEH / ARABIC LIGATURE MEEM WITH MEEM WITH YEH
      [[0x0646],[0xFC8D,0xFEE5,0xFEE6,0xFEE7,0xFEE8]], # ARABIC LIGATURE NOON WITH NOON / ARABIC LETTER NOON
      [[0x0646,0x0647],[0xFCD6,0xFCEF]], # ARABIC LIGATURE NOON WITH HEH
      [[0x0646,0x0649],[0xFC4F,0xFC8E]], # ARABIC LIGATURE NOON WITH ALEF MAKSURA
      [[0x0646,0x064A],[0xFC50,0xFC8F,0xFC94]], # ARABIC LIGATURE NOON WITH YEH / ARABIC LIGATURE YEH WITH NOON
      [[0x0647],[0xFEE9,0xFEEA,0xFEEB,0xFEEC]], # ARABIC LETTER HEH
      [[0x0647,0x0649],[0xFC53]], # ARABIC LIGATURE HEH WITH ALEF MAKSURA
      [[0x0647,0x064A],[0xFC54,0xFCDE,0xFCF1]], # ARABIC LIGATURE HEH WITH YEH / ARABIC LIGATURE YEH WITH HEH
      [[0x0647,0x0670],[0xFCD9]], # ARABIC LIGATURE HEH WITH SUPERSCRIPT ALEF
      [[0x0648],[0xFEED,0xFEEE]], # ARABIC LETTER WAW
      [[0x0649],[0xFBE8,0xFBE9,0xFEEF,0xFEF0]], # ARABIC LETTER UIGHUR KAZAKH KIRGHIZ ALEF MAKSURA / ARABIC LETTER ALEF MAKSURA
      [[0x0649,0x064A],[0xFC59,0xFC95]], # ARABIC LIGATURE YEH WITH ALEF MAKSURA
      [[0x0649,0x0670],[0xFC5D,0xFC90]], # ARABIC LIGATURE ALEF MAKSURA WITH SUPERSCRIPT ALEF
      [[0x064A],[0xFC5A,0xFC96,0xFEF1,0xFEF2,0xFEF3,0xFEF4]], # ARABIC LIGATURE YEH WITH YEH / ARABIC LETTER YEH
      [[0x064B],[0xFE70]], # ARABIC FATHATAN
      [[0x064C],[0xFE72]], # ARABIC DAMMATAN
      [[0x064C,0x0651],[0xFC5E]], # ARABIC LIGATURE SHADDA WITH DAMMATAN
      [[0x064D],[0xFE74]], # ARABIC KASRATAN
      [[0x064D,0x0651],[0xFC5F]], # ARABIC LIGATURE SHADDA WITH KASRATAN
      [[0x064E],[0xFE76]], # ARABIC FATHA
      [[0x064E,0x0651],[0xFC60]], # ARABIC LIGATURE SHADDA WITH FATHA
      [[0x064F],[0xFE78]], # ARABIC DAMMA
      [[0x064F,0x0651],[0xFC61]], # ARABIC LIGATURE SHADDA WITH DAMMA
      [[0x0650],[0xFE7A]], # ARABIC KASRA
      [[0x0650,0x0651],[0xFC62]], # ARABIC LIGATURE SHADDA WITH KASRA
      [[0x0651],[0xFE7C]], # ARABIC SHADDA
      [[0x0651,0x0670],[0xFC63]], # ARABIC LIGATURE SHADDA WITH SUPERSCRIPT ALEF
      [[0x0652],[0xFE7E]], # ARABIC SUKUN
      [[0x0671],[0xFB50,0xFB51]], # ARABIC LETTER ALEF WASLA
      [[0x0677],[0xFBDD]], # ARABIC LETTER U WITH HAMZA ABOVE
      [[0x0679],[0xFB66,0xFB67,0xFB68,0xFB69]], # ARABIC LETTER TTEH
      [[0x067A],[0xFB5E,0xFB5F,0xFB60,0xFB61]], # ARABIC LETTER TTEHEH
      [[0x067B],[0xFB52,0xFB53,0xFB54,0xFB55]], # ARABIC LETTER BEEH
      [[0x067E],[0xFB56,0xFB57,0xFB58,0xFB59]], # ARABIC LETTER PEH
      [[0x067F],[0xFB62,0xFB63,0xFB64,0xFB65]], # ARABIC LETTER TEHEH
      [[0x0680],[0xFB5A,0xFB5B,0xFB5C,0xFB5D]], # ARABIC LETTER BEHEH
      [[0x0683],[0xFB76,0xFB77,0xFB78,0xFB79]], # ARABIC LETTER NYEH
      [[0x0684],[0xFB72,0xFB73,0xFB74,0xFB75]], # ARABIC LETTER DYEH
      [[0x0686],[0xFB7A,0xFB7B,0xFB7C,0xFB7D]], # ARABIC LETTER TCHEH
      [[0x0687],[0xFB7E,0xFB7F,0xFB80,0xFB81]], # ARABIC LETTER TCHEHEH
      [[0x0688],[0xFB88,0xFB89]], # ARABIC LETTER DDAL
      [[0x068C],[0xFB84,0xFB85]], # ARABIC LETTER DAHAL
      [[0x068D],[0xFB82,0xFB83]], # ARABIC LETTER DDAHAL
      [[0x068E],[0xFB86,0xFB87]], # ARABIC LETTER DUL
      [[0x0691],[0xFB8C,0xFB8D]], # ARABIC LETTER RREH
      [[0x0698],[0xFB8A,0xFB8B]], # ARABIC LETTER JEH
      [[0x06A4],[0xFB6A,0xFB6B,0xFB6C,0xFB6D]], # ARABIC LETTER VEH
      [[0x06A6],[0xFB6E,0xFB6F,0xFB70,0xFB71]], # ARABIC LETTER PEHEH
      [[0x06A9],[0xFB8E,0xFB8F,0xFB90,0xFB91]], # ARABIC LETTER KEHEH
      [[0x06AD],[0xFBD3,0xFBD4,0xFBD5,0xFBD6]], # ARABIC LETTER NG
      [[0x06AF],[0xFB92,0xFB93,0xFB94,0xFB95]], # ARABIC LETTER GAF
      [[0x06B1],[0xFB9A,0xFB9B,0xFB9C,0xFB9D]], # ARABIC LETTER NGOEH
      [[0x06B3],[0xFB96,0xFB97,0xFB98,0xFB99]], # ARABIC LETTER GUEH
      [[0x06BA],[0xFB9E,0xFB9F]], # ARABIC LETTER NOON GHUNNA
      [[0x06BB],[0xFBA0,0xFBA1,0xFBA2,0xFBA3]], # ARABIC LETTER RNOON
      [[0x06BE],[0xFBAA,0xFBAB,0xFBAC,0xFBAD]], # ARABIC LETTER HEH DOACHASHMEE
      [[0x06C0],[0xFBA4,0xFBA5]], # ARABIC LETTER HEH WITH YEH ABOVE
      [[0x06C1],[0xFBA6,0xFBA7,0xFBA8,0xFBA9]], # ARABIC LETTER HEH GOAL
      [[0x06C5],[0xFBE0,0xFBE1]], # ARABIC LETTER KIRGHIZ OE
      [[0x06C6],[0xFBD9,0xFBDA]], # ARABIC LETTER OE
      [[0x06C7],[0xFBD7,0xFBD8]], # ARABIC LETTER U
      [[0x06C8],[0xFBDB,0xFBDC]], # ARABIC LETTER YU
      [[0x06C9],[0xFBE2,0xFBE3]], # ARABIC LETTER KIRGHIZ YU
      [[0x06CB],[0xFBDE,0xFBDF]], # ARABIC LETTER VE
      [[0x06CC],[0xFBFC,0xFBFD,0xFBFE,0xFBFF]], # ARABIC LETTER FARSI YEH
      [[0x06D0],[0xFBE4,0xFBE5,0xFBE6,0xFBE7]], # ARABIC LETTER E
      [[0x06D2],[0xFBAE,0xFBAF]], # ARABIC LETTER YEH BARREE
      [[0x06D3],[0xFBB0,0xFBB1]], # ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
      [[0],[0]]
    ].each do |u,l|
      if (unicodes & u) == u
        unicodes += l
      end
    end
    unicodes.uniq
  end

  def mirror_brackes(unicodes)
    [['<','>'],['(',')'],['[',']'],['{','}']].each do |l,r|
      has_l = unicodes.include?(l.ord)
      has_r = unicodes.include?(r.ord)
      unicodes.push(r.ord) if has_l and !has_r
      unicodes.push(l.ord) if has_r and !has_l
    end
    unicodes
  end

  def add_thai(unicodes)
    # Add Thai unicodes which are used to calculate diacritic positions when the Thai characters have special shapes.
    [[0x0E09, 0x0E08],[0x0E13,0x0E0C],[0x0E19,0x0E18],[0x0E1B, 0x0E1A],[0x0E1D,0x0E1C],[0x0E1F,0x0E1E],[0x0E33,0x0E32],[0x0E33,0x0E4D]].each do |has,need|
      has_unicode = unicodes.include?(has)
      has_needed = unicodes.include?(need)
      unicodes.push(need) if has_unicode and !has_needed
    end
    unicodes
  end

  def check_for_rtl(unicodes)
    return if @text_entries.is_rtl # No need to look for unicode if RTL already detected
    # Look for hebrew (0x0590-0x05ff) or arabic (0x0600-0x06ff) + arabic ligatures (0xFE70-0xFEFF)
    @text_entries.unicode_uses_rtl if unicodes.any?{|u| u.between?(0x0590, 0x05FF) || u.between?(0x0600, 0x06FF) || u.between?(0xFE70, 0xFEFE) }
  end

  def decode_ranges(str)
    result = []
    while str.length > 0
      char_range = str.match(/^(.)-(.)(.*)$/)
      if char_range
        first_char = char_range[1]
        last_char = char_range[2]
        result += (first_char.ord .. last_char.ord).to_a
        str = char_range[3]
      else
        num_range = str.match(/^(0[xX][0-9a-fA-F]+|\d+)(?:\.0+)?-(0[xX][0-9a-fA-F]+|\d+)(?:\.0+)?(.*)$/)
        if num_range
          first_num = Integer(num_range[1])
          last_num = Integer(num_range[2])
          result += (first_num..last_num).to_a
          str = num_range[3]
        else
          num = str.match(/^(0[xX][0-9a-fA-F]+|\d+)(?:\.0+)?(.*)/)
          if num
            # Check for typo such as 0,2-9
            if num[1].length == 1
              result += [ num[1].ord ]
            else
              result += [ Integer(num[1]) ]
            end
            str = num[2]
          else
            abort "Unexpected character at #{str}"
          end
        end
      end
      if str.length > 0
        if str[0] == ','
          str = str[1..-1]
        else
          abort "Please separate wildcard ranges with ','"
        end
      end
    end
    result
  end

  def run
    typographies_identical = @typographies.select{ |t| t.font_file == @unique_typography.font_file &&
                                                       t.font_size == @unique_typography.font_size &&
                                                       t.bpp == @unique_typography.bpp }
    typography_names = typographies_identical.map{ |t| t.name }.uniq

    # Find a typography with a fallback character
    typography_with_fallback_character = typographies_identical.find { |t| t.fallback_character }
    if typography_with_fallback_character
      # Now get the actual fallback character (or 'skip')
      typography_fallback_character = typography_with_fallback_character.fallback_character
      # Check to see if one of the other typographes has a different fallback character
      index = typographies_identical.find_index{ |t| t.fallback_character && t.fallback_character != typography_fallback_character }
      if index
        abort "The fallback character differs for typography \"#{typography_with_fallback_character.name}\" and typography \"#{typographies_identical[index].name}\""
      end
      # set all fallback characters to the same character
      typographies_identical.each { |t| t.fallback_character = typography_fallback_character }
    end

    # Find a typography with a ellipsis character
    typography_with_ellipsis_character = typographies_identical.find { |t| t.ellipsis_character }
    if typography_with_ellipsis_character
      # Now get the actual ellipsis character (or 'skip')
      typography_ellipsis_character = typography_with_ellipsis_character.ellipsis_character
      # Check to see if one of the other typographes has a different ellipsis character
      index = typographies_identical.find_index{ |t| t.ellipsis_character && t.ellipsis_character != typography_ellipsis_character }
      if index
        abort "The ellipsis character differs for typography \"#{typography_with_ellipsis_character.name}\" and typography \"#{typographies_identical[index].name}\""
      end
      # set all ellipsis characters to the same character
      typographies_identical.each { |t| t.ellipsis_character = typography_ellipsis_character }
    end

    all_translations = typography_names.map{ |typography_name| @text_entries.collect{ |entry| entry.translations_with_typography(typography_name) }.flatten }.flatten

    unicodes = all_translations.map(&:unicodes).flatten.uniq.sort

    typographies_identical.each do |t|
      fbc = t.fallback_character
      fbcUnicode = 0
      if fbc
        if fbc.downcase == 'skip'
          fbcUnicode = 0xFEFF
        elsif fbc.length == 1
          fbcUnicode = fbc[0].ord
        else
          begin
            fbcUnicode = Integer(fbc.gsub(/\.0*$/,''))
          rescue
            fail "Please only specify one character or ('skip') as Fallback Character, typography \"#{typography_with_fallback_character.name}\" has Fallback Character \"#{typography_with_fallback_character.fallback_character}\""
          end
        end
        unicodes += [ fbcUnicode ]
      end
      t.fallback_character = fbcUnicode

      tec = t.ellipsis_character
      tecUnicode = 0
      if tec
        if tec.length == 1
          tecUnicode = tec[0].ord
        else
          begin
            tecUnicode = Integer(tec.gsub(/\.0*$/,''))
          rescue
            fail "Please only specify one character as Ellipsis Character for typography \"#{typography_with_fallback_character.name}\""
          end
        end
        unicodes += [ tecUnicode ]
      end
      t.ellipsis_character = tecUnicode
    end
    typographies_identical.each{ |t|
      if t.wildcard_characters
        t.wildcard_characters.to_s.split('').each { |c|
          unicodes += [ c[0].ord ]
        }
      end
      if t.wildcard_ranges
        unicodes += decode_ranges(t.wildcard_ranges)
      end
    }
    
    unicodes = convert_to_contextual_forms(unicodes)

    unicodes = mirror_brackes(unicodes)

    unicodes = add_thai(unicodes)

    unicodes.delete(0x0002) # TouchGFX wildcard character
    unicodes.delete(0x200B) # ZERO WIDTH SPACE
    unicodes.delete(0xFEFF) # ZERO WIDTH NO-BREAK SPACE

    unicodes = unicodes.uniq.sort

    check_for_rtl(unicodes)

    FileIO.write_file_silent(File.join(@output_directory, "UnicodeList#{@unique_typography.cpp_name}_#{@unique_typography.font_size}_#{@unique_typography.bpp}.txt"), unicodes.join(LINE_ENDINGS) )
  end
end
