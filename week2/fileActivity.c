#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 100
#define MAX_CONTENT 1024

FILE *file = NULL;
char filename[MAX_FILENAME];

void openFile() {
    if (file != NULL) {
        printf("File đã được mở rồi. Hãy đóng trước khi mở file mới.\n");
        return;
    }
    printf("Nhập tên file cần mở: ");
    fgets(filename, MAX_FILENAME, stdin);
    filename[strcspn(filename, "\n")] = '\0';  // Xóa ký tự newline

    file = fopen(filename, "a+");  // Mở để đọc và ghi, tạo nếu chưa có
    if (file == NULL) {
        perror("Không thể mở file");
    } else {
        printf("Đã mở file '%s'\n", filename);
    }
}

void readFile() {
    if (file == NULL) {
        printf("Chưa mở file nào.\n");
        return;
    }

    rewind(file);  // Đặt lại vị trí con trỏ về đầu file
    printf("Nội dung file '%s':\n", filename);
    char buffer[MAX_CONTENT];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }
    printf("\n");
}

void writeFile() {
    if (file == NULL) {
        printf("Chưa mở file nào.\n");
        return;
    }

    char input[MAX_CONTENT];
    printf("Nhập nội dung cần ghi (kết thúc bằng dòng trống):\n");
    while (1) {
        fgets(input, sizeof(input), stdin);
        if (strcmp(input, "\n") == 0) break;  // Dòng trống để kết thúc
        fputs(input, file);
    }

    fflush(file);  // Đảm bảo nội dung được ghi ra đĩa
    printf("Đã ghi nội dung vào file.\n");
}

void closeFile() {
    if (file != NULL) {
        fclose(file);
        file = NULL;
        printf("Đã đóng file '%s'\n", filename);
    } else {
        printf("Không có file nào đang mở.\n");
    }
}

int main() {
    char choice[10];

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Open file\n");
        printf("2. Read file\n");
        printf("3. Write file\n");
        printf("4. Close file\n");
        printf("Nhập số để chọn chức năng, hoặc 'exit' để thoát: ");
        
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = '\0';  // Xóa ký tự newline

        if (strcmp(choice, "exit") == 0) {
            break;
        }

        switch (atoi(choice)) {
            case 1:
                openFile();
                break;
            case 2:
                readFile();
                break;
            case 3:
                writeFile();
                break;
            case 4:
                closeFile();
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }

    }

    // Đảm bảo file được đóng khi thoát chương trình
    if (file != NULL) {
        fclose(file);
    }

    printf("Thoát chương trình.\n");
    return 0;
}
