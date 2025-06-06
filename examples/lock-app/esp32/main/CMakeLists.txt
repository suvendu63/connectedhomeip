#
# Copyright (c) 2021 Project CHIP Authors
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

get_filename_component(CHIP_ROOT ${CMAKE_SOURCE_DIR}/third_party/connectedhomeip REALPATH)
get_filename_component(APP_COMMON_GEN_DIR ${CHIP_ROOT}/zzz_generated/app-common/app-common/zap-generated REALPATH)

if(CONFIG_ENABLE_PW_RPC)
  if(${IDF_VERSION_MAJOR} LESS 5)
    set(freertos_include_path "${IDF_PATH}/components/freertos/include/freertos")
  else()
    set(freertos_include_path "${IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos")
  endif()

  idf_component_register(INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CHIP_ROOT}/examples/platform/esp32/pw_sys_io/public"
    "${CHIP_ROOT}/examples/platform/esp32"
    "${CHIP_ROOT}/examples/platform/esp32/lock"
    "${CHIP_ROOT}/examples/providers"
    "${CHIP_ROOT}/examples/common"
    "${CHIP_ROOT}/examples/common/pigweed"
    "${CHIP_ROOT}/examples/common/pigweed/esp32"
    "${CHIP_ROOT}/src/lib/support"
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${freertos_include_path}"
    SRC_DIRS
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CHIP_ROOT}/examples/providers"
    "${CHIP_ROOT}/examples/platform/esp32"
    "${CHIP_ROOT}/examples/platform/esp32/common"
    "${CHIP_ROOT}/examples/platform/esp32/lock"
    "${CHIP_ROOT}/examples/common/pigweed"
    "${CHIP_ROOT}/examples/common/pigweed/esp32"
    "${APP_COMMON_GEN_DIR}/attributes"
    "${APP_COMMON_GEN_DIR}"
  )

  set(PIGWEED_ROOT "${CHIP_ROOT}/third_party/pigweed/repo")
  include(${PIGWEED_ROOT}/pw_build/pigweed.cmake)
  include(${PIGWEED_ROOT}/pw_protobuf_compiler/proto.cmake)
  set(dir_pw_third_party_nanopb "${CHIP_ROOT}/third_party/nanopb/repo" CACHE STRING "" FORCE)

  pw_proto_library(descriptor_service
    SOURCES
    ${CHIP_ROOT}/examples/common/pigweed/protos/descriptor_service.proto
    PREFIX
    descriptor_service
    STRIP_PREFIX
    ${CHIP_ROOT}/examples/common/pigweed/protos
    DEPS
    pw_protobuf.common_proto
  )

  pw_proto_library(device_service
    SOURCES
    ${CHIP_ROOT}/examples/common/pigweed/protos/device_service.proto
    INPUTS
    ${CHIP_ROOT}/examples/common/pigweed/protos/device_service.options
    PREFIX
    device_service
    STRIP_PREFIX
    ${CHIP_ROOT}/examples/common/pigweed/protos
    DEPS
    pw_protobuf.common_proto
  )

  pw_proto_library(button_service
    SOURCES
    ${CHIP_ROOT}/examples/common/pigweed/protos/button_service.proto
    PREFIX
    button_service
    STRIP_PREFIX
    ${CHIP_ROOT}/examples/common/pigweed/protos
    DEPS
    pw_protobuf.common_proto
  )

  pw_proto_library(locking_service
    SOURCES
    ${CHIP_ROOT}/examples/common/pigweed/protos/locking_service.proto
    PREFIX
    locking_service
    STRIP_PREFIX
    ${CHIP_ROOT}/examples/common/pigweed/protos
    DEPS
    pw_protobuf.common_proto
  )

  target_link_libraries(${COMPONENT_LIB} PUBLIC
    attributes_service.nanopb_rpc
    descriptor_service.nanopb_rpc
    device_service.nanopb_rpc
    button_service.nanopb_rpc
    locking_service.nanopb_rpc
    pw_checksum
    pw_hdlc
    pw_log
    pw_rpc.server
    pw_sys_io
  )

  target_compile_options(${COMPONENT_LIB} PRIVATE
    "-DPW_RPC_ATTRIBUTE_SERVICE=2"
    "-DPW_RPC_BUTTON_SERVICE=1"
    "-DPW_RPC_DESCRIPTOR_SERVICE=1"
    "-DPW_RPC_DEVICE_SERVICE=1"
    "-DPW_RPC_LOCKING_SERVICE=1")

else(CONFIG_ENABLE_PW_RPC)
  idf_component_register(PRIV_INCLUDE_DIRS
    "${CHIP_ROOT}/examples/providers"
    "${CMAKE_CURRENT_LIST_DIR}/include"
    SRC_DIRS
    "${CMAKE_CURRENT_LIST_DIR}"
    "${APP_COMMON_GEN_DIR}/attributes"
    "${APP_COMMON_GEN_DIR}"
    "${CHIP_ROOT}/examples/providers"
    "${CHIP_ROOT}/examples/platform/esp32/shell_extension"
    "${CHIP_ROOT}/examples/platform/esp32/common"
    "${CHIP_ROOT}/examples/platform/esp32/lock"
  )

  set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
  target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
  target_compile_options(${COMPONENT_LIB} PUBLIC
    "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>"
  )
  target_compile_options(${COMPONENT_LIB} PUBLIC
    "-DCHIP_CONFIG_ENABLE_ACL_EXTENSIONS=1"
  )
endif(CONFIG_ENABLE_PW_RPC)

include(${CHIP_ROOT}/src/app/chip_data_model.cmake)
chip_configure_data_model(${COMPONENT_LIB}
  ZAP_FILE ${CHIP_ROOT}/examples/lock-app/lock-common/lock-app.zap
)
