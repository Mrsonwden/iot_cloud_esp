TEST_PROGRAM=test_nvs
all: $(TEST_PROGRAM)

SOURCE_FILES = \
	esp_error_check_stub.cpp \
	$(addprefix ../src/, \
		nvs_types.cpp \
		nvs_api.cpp \
		nvs_page.cpp \
		nvs_pagemanager.cpp \
		nvs_storage.cpp \
		nvs_item_hash_list.cpp \
		nvs_encr.cpp \
		nvs_ops.cpp \
	) \
	spi_flash_emulation.cpp \
	test_compressed_enum_table.cpp \
	test_spi_flash_emulation.cpp \
	test_intrusive_list.cpp \
	test_nvs.cpp \
	crc.cpp \
	main.cpp

CPPFLAGS += -I../include -I../src -I./ -I../../esp32/include -I ../../mbedtls/mbedtls/include -I ../../spi_flash/include -I ../../../tools/catch -fprofile-arcs -ftest-coverage -DCONFIG_NVS_ENCRYPTION
CFLAGS += -fprofile-arcs -ftest-coverage
CXXFLAGS += -std=c++11 -Wall -Werror
LDFLAGS += -lstdc++ -Wall -fprofile-arcs -ftest-coverage

OBJ_FILES = $(SOURCE_FILES:.cpp=.o)

COVERAGE_FILES = $(OBJ_FILES:.o=.gc*)

$(OBJ_FILES): %.o: %.cpp

$(TEST_PROGRAM): $(OBJ_FILES)
	$(MAKE) -C ../../mbedtls/mbedtls/ lib
	g++ $(LDFLAGS) -o $(TEST_PROGRAM) $(OBJ_FILES) ../../mbedtls/mbedtls/library/libmbedcrypto.a

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes exclude:[long]

long-test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM) -d yes

$(COVERAGE_FILES): $(TEST_PROGRAM) long-test

coverage.info: $(COVERAGE_FILES)
	find ../src/ -name "*.gcno" -exec gcov -r -pb {} +
	lcov --capture --directory ../src --no-external --output-file coverage.info

coverage_report: coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Coverage report is in coverage_report/index.html"

clean:
	$(MAKE) -C ../../mbedtls/mbedtls/ clean
	rm -f $(OBJ_FILES) $(TEST_PROGRAM)
	rm -f $(COVERAGE_FILES) *.gcov
	rm -rf coverage_report/
	rm -f coverage.info
	rm ../nvs_partition_generator/partition_single_page.bin
	rm ../nvs_partition_generator/partition_multipage_blob.bin
	rm ../nvs_partition_generator/partition_encrypted.bin
	rm ../nvs_partition_generator/partition_encrypted_using_keygen.bin
	rm ../nvs_partition_generator/partition_encrypted_using_keyfile.bin

.PHONY: clean all test long-test
