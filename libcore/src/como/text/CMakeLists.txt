#=========================================================================
# Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

set(TEXT_DIR ${LIBCORE_COMO_SRC_DIR}/text)

include_directories(
    ./
    ../
    ../../../inc
    ${INC_DIR}
    ${LIBCORE_OBJ_DIR})

set(SOURCES
    AttributedCharacterIteratorAttribute.cpp
    AttributedCharacterIteratorAttributeFactory.cpp
    AttributedString.cpp
    CalendarBuilder.cpp
    CharacterIteratorFieldDelegate.cpp
    ChoiceFormat.cpp
    DateFormat.cpp
    DateFormatSymbols.cpp
    DecimalFormat.cpp
    DecimalFormatSymbols.cpp
    DontCareFieldPosition.cpp
    FieldPosition.cpp
    Format.cpp
    MessageFormat.cpp
    NumberFormat.cpp
    NumberFormatFactory.cpp
    ParsePosition.cpp
    SimpleDateFormat.cpp
    text.cpp)

add_library(como_text STATIC
    ${SOURCES})
add_dependencies(como_text build_libcore)