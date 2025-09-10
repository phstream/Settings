#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../ini.h"

#define MAX_LINE_LENGTH (20)

int main(void) {
    int version = ini_version();
    assert(version >= 1000000 && version < 2000000);
    printf("✅ Test passed: Version = %i\n", version);
    assert(strcmp(ini_error_string(1000), "No Error") == 0);
    assert(strcmp(ini_error_string(0), "No Error") == 0);
    assert(strcmp(ini_error_string(-1), "No Error") != 0);
    assert(strcmp(ini_error_string(-999), "Unknown Error") == 0);
    assert(strcmp(ini_error_string(1000), "Unknown Error") != 0);
    printf("✅ Test passed: Error string = %s\n", ini_error_string(1000));
    const char inifile1[] = "./test/test1.ini";
    remove(inifile1);
    int result = ini_write_key(inifile1, "MySection", "pi", "3.14", "Definition of PI");
    printf("write = %s\n", ini_error_string(result));
    assert(result == 0);
    result = ini_write_key(inifile1, "AnotherSection", "path", "\"C:\\\\path\\\\to\\\\another.txt\"", "Just Another File");
    printf("write = %s\n", ini_error_string(result));
    assert(result == 0);
    result = ini_write_key(inifile1, "MySection", "path", "C:\\path\\to\\file.txt", "Important File");
    printf("write = %s\n", ini_error_string(result));
    assert(result == 0);
    result = ini_write_key(inifile1, "MySection", "pi", "3.14159", "Unimportant File");
    printf("update = %s\n", ini_error_string(result));
    assert(result == 0);
    printf("✅ Test passed: write\n");
    char buffer[MAX_LINE_LENGTH];
    result = ini_read_key(inifile1, "MySection", "pi", buffer, MAX_LINE_LENGTH);
    printf("read: \'%s\' = %s\n", buffer, ini_error_string(result));
    assert(result == 7);
    assert(strcmp(buffer, "3.14159") == 0);
    result = ini_read_key(inifile1, "MySection", "pi", buffer, 8);
    printf("read: \'%s\' = %i (\'%s\')\n", buffer, result, ini_error_string(result));
    assert(result == 7);
    assert(strcmp(buffer, "3.14159") == 0);
    result = ini_read_key(inifile1, "MySection", "pi", buffer, 7);
    printf("read: \'%s\' = %i (\'%s\')\n", buffer, result, ini_error_string(result));
    assert(result == 6);
    assert(strlen(buffer) == 6);
    result = ini_read_key(inifile1, "MySection", "py", buffer,  MAX_LINE_LENGTH);
    printf("read: \'%s\' = %i (\'%s\')\n", buffer, result, ini_error_string(result));
    assert(result == -4);
    assert(strlen(buffer) == 0);
    printf("✅ Test passed: read\n");
    result = ini_read_key("./does_not_exist.ini", "Sec", "key", buffer, MAX_LINE_LENGTH);
    printf("bad read: \'%s\' = %i (\'%s\')\n", buffer, result, ini_error_string(result));
    assert(result < 0);
    printf("✅ Test passed: missing ini\n");

    return 0;
}