#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PASSWORD 1234

typedef struct {
    char name[50];
    int code;
    int stock;
} Product;

void enterPassword() {
    printf("==================================================================");
    printf("\nSelamat datang di Sistem Informasi Manajemen Inventaris\n");
    printf("==================================================================\n");
    
    int password;
    printf("Masukkan 4-Digit PIN anda: ");
    scanf("%d", &password);
    
    if (password != PASSWORD) {
        printf("PIN salah, akses ditolak.\n");
        exit(1); // Keluar dari program
    }
}

void writeUpdatedProducts(FILE **file, const char *tempFileName) {
    fclose(*file); 
    remove("data.dat"); 
    rename(tempFileName, "data.dat"); 
    *file = fopen("data.dat", "ab+"); 
    if (*file == NULL) {
        printf("Error reopening the data file.\n");
        exit(1); 
    }
}

void addProduct(FILE *file) {
    Product newProduct;
    int found = 0;
    
    printf("Masukkan Nama Barang: ");
    scanf("%s", newProduct.name);
    printf("Masukkan Kode Barang: ");
    scanf("%d", &newProduct.code);
    printf("Stok Barang: ");
    scanf("%d", &newProduct.stock);

    Product currentProduct;
    rewind(file);
    while (fread(&currentProduct, sizeof(Product), 1, file)) {
        if (strcmp(currentProduct.name, newProduct.name) == 0 && currentProduct.code == newProduct.code) {
            currentProduct.stock += newProduct.stock;
            fseek(file, -sizeof(Product), SEEK_CUR);
            fwrite(&currentProduct, sizeof(Product), 1, file);
            fflush(file); // Flush perubahan ke file
            found = 1;
            break;
        }
    }

    if (!found) {
        fwrite(&newProduct, sizeof(Product), 1, file);
        fflush(file);
    }

    printf("Barang berhasil ditambahkan!\n");
}

// Fungsi untuk menghapus barang dari list
void removeProduct(FILE **file, int code) {
    FILE *tempFile = fopen("temp.dat", "wb");
    if (!tempFile) {
        printf("Error creating temporary file.\n");
        return;
    }

    Product currentProduct;
    rewind(*file);
    while (fread(&currentProduct, sizeof(Product), 1, *file)) {
        if (currentProduct.code != code) {
            fwrite(&currentProduct, sizeof(Product), 1, tempFile);
        }
    }

    fclose(tempFile);
    writeUpdatedProducts(file, "temp.dat");
    printf("Barang berhasil dihapus!\n");
}

// Fungsi untuk menampilkan daftar barang
void viewProducts(FILE *file) {
    Product currentProduct;
    rewind(file);
    while (fread(&currentProduct, sizeof(Product), 1, file)) {
        printf("Name: %s, Code: %d, Stock: %d\n", currentProduct.name, currentProduct.code, currentProduct.stock);
    }
}

//Fungsi untuk menambah/mengurangi stok barang
void adjustProductStock(FILE **file, int code, int quantity, int increase) {
    FILE *tempFile = fopen("temp.dat", "wb");
    if (!tempFile) {
        printf("Error creating temporary file.\n");
        return;
    }

    Product currentProduct;
    rewind(*file);
    while (fread(&currentProduct, sizeof(Product), 1, *file)) {
        if (currentProduct.code == code) {
            currentProduct.stock += (increase ? quantity : -quantity);
        }
        fwrite(&currentProduct, sizeof(Product), 1, tempFile);
    }

    fclose(tempFile);
    writeUpdatedProducts(file, "temp.dat");
    printf("Stok barang berhasil %s!\n", increase ? "ditambahkan" : "dikurangi");
}

int main() {
    FILE *file = fopen("data.dat", "ab+");
    if (file == NULL) {
        printf("Error saat membuka data.\n");
        return 1;
    }

    enterPassword();

    int choice, code, quantity;
    do {
        printf("=================================================================================\n");
        printf("Selamat datang di Menu\n");
        printf("=================================================================================\n");
        printf("Pilih menu yang ingin dipilih dengan memasukkan nomor menu\n");
        printf("\n1. Tambah Barang\n2. Hapus Barang\n3. Lihat Barang\n4. Tambah Stok Barang\n5. Kurangi Stok Barang\n6. Keluar\n");
        printf("Masukkan input menu: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                rewind(file);
                addProduct(file);
                break;
            case 2:
                printf("Masukkan kode barang: ");
                scanf("%d", &code);
                removeProduct(&file, code);
                break;
            case 3:
                rewind(file);
                viewProducts(file);
                break;
            case 4:
                printf("Masukkan kode barang: ");
                scanf("%d", &code);
                printf("Masukkan jumlah yang ingin ditambah: ");
                scanf("%d", &quantity);
                adjustProductStock(&file, code, quantity, 1);
                break;
            case 5:
                printf("Masukkan kode barang: ");
                scanf("%d", &code);
                printf("Masukkan jumlah barang yang berkurang: ");
                scanf("%d", &quantity);
                adjustProductStock(&file, code, quantity, 0);
                break;
            case 6:
                printf("Menutup program ...\n");
                break;
            default:
                printf("Pilihan tidak valid, coba lagi\n");
        }
    } while (choice != 6);

    fclose(file);
    return 0;
}
