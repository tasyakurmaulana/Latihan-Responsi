//Tasyakur Maulana Istna
//124240053
#include <iostream>
using namespace std;

struct Video {
    string judul;
    int durasi;
    int status;
    bool dihapus = false; 
};

struct AksiUndo {
    string tipe; 
    Video data;
};

struct StackUndo {
    AksiUndo data;
    StackUndo* next;
};
StackUndo* atasUndo = nullptr;

void pushUndo(string tipe, Video data) {
    StackUndo* baru = new StackUndo;
    baru->data.tipe = tipe;
    baru->data.data = data;
    baru->next = atasUndo;
    atasUndo = baru;
}

bool undoKosong() {
    return atasUndo == nullptr;
}

AksiUndo popUndo() {
    if (undoKosong()) return {"", {"", 0, 0}};
    StackUndo* temp = atasUndo;
    atasUndo = atasUndo->next;
    AksiUndo aksi = temp->data;
    delete temp;
    return aksi;
}

struct Node {
    Video data;
    Node* left;
    Node* right;
};

Node* insert(Node* root, Video vid) {
    if (!root) return new Node{vid, nullptr, nullptr};
    if (vid.judul < root->data.judul)
        root->left = insert(root->left, vid);
    else if (vid.judul > root->data.judul)
        root->right = insert(root->right, vid);
    else
        cout << "Judul video yang anda masukkan sudah ada!\n";
    return root;
}

Node* cari(Node* root, string judul) {
    if (!root || root->data.judul == judul) return root;
    return (judul < root->data.judul) ? cari(root->left, judul) : cari(root->right, judul);
}

void inorder(Node* root) {
    if (!root) return;
    inorder(root->left);
    cout << "=========================\n";
    if (!root->data.dihapus) {
        cout << "Judul   : " << root->data.judul << endl;
        cout << "Durasi  : " << root->data.durasi << endl;
        cout << "Status  : ";
        if (root->data.status == 0) cout << "Tersedia\n";
        else if (root->data.status == 1) cout << "Dalam antrean\n";
        else cout << "Sedang diputar\n";
        cout << "=========================\n";
    }
    inorder(root->right);
}

struct QueueNode {
    Video* data;
    QueueNode* next;
};
QueueNode *depan = nullptr, *belakang = nullptr;

void enqueue(Video* vid) {
    QueueNode* baru = new QueueNode{vid, nullptr};
    if (!depan) depan = baru;
    else belakang->next = baru;
    belakang = baru;
}

void dequeue() {
    if (!depan) return;
    QueueNode* hapus = depan;
    depan = depan->next;
    delete hapus;
    if (!depan) belakang = nullptr;
}

bool queueKosong() { return depan == nullptr; }

bool adaDiPlaylist(string judul) {
    QueueNode* bantu = depan;
    while (bantu) {
        if (bantu->data->judul == judul) return true;
        bantu = bantu->next;
    }
    return false;
}

struct StackNode {
    Video* data;
    StackNode* next;
};
StackNode* atasRiwayat = nullptr;

void push(StackNode*& top, Video* vid) {
    StackNode* baru = new StackNode{vid, top};
    top = baru;
}

void pop(StackNode*& top) {
    if (!top) return;
    StackNode* hapus = top;
    top = top->next;
    delete hapus;
}

void tampilkanRiwayat(StackNode* top) {
    if (!top) {
        cout << "Riwayat masih kosong.\n";
        return;
    }
    StackNode* bantu = top;
    while (bantu) {
        cout << bantu->data->judul << " (" << bantu->data->durasi << " menit)\n";
        bantu = bantu->next;
    }
}

void tambahKePlaylist(Node* root, string judul) {
    Node* vid = cari(root, judul);
    if (!vid) {
        cout << "Video tidak ditemukan.\n";
        return;
    }
    if (vid->data.status != 0) {
        cout << "Video sudah dalam playlist atau sedang diputar.\n";
        return;
    }
    pushUndo("playlist", vid->data);
    vid->data.status = queueKosong() ? 2 : 1;
    enqueue(&vid->data);
    cout << "Video berhasil ditambahkan ke playlist.\n";
}

void tontonVideo() {
    if (queueKosong()) {
        cout << "Tidak ada video yang sedang diputar.\n";
        return;
    }
    Video* v = depan->data;
    cout << "Menonton: " << v->judul << endl;
    pushUndo("tonton", *v);
    push(atasRiwayat, v);
    v->status = 0;
    dequeue();
    if (depan) depan->data->status = 2;
}

void cariVideo(Node* root, string judul) {
    Node* hasil = cari(root, judul);
    if (hasil) {
        cout << "Video ditemukan: " << hasil->data.judul << " (" << hasil->data.durasi << " menit)\n";
    } else {
        cout << "Video tidak ditemukan.\n";
    }
}

void hapusDariPlaylist(string judul) {
    QueueNode *curr = depan, *prev = nullptr;
    while (curr) {
        if (curr->data->judul == judul) {
            if (prev) prev->next = curr->next;
            else depan = curr->next;
            if (curr == belakang) belakang = prev;
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

Node* hapusVideo(Node* root, string judul) {
    Node* node = cari(root, judul);
    if (!node) return root;
    if (node->data.status == 1 || node->data.status == 2) {
        char konfirmasi;
        cout << "Video sedang diputar/masih dalam antrean. Yakin ingin menghapus? (y/t): ";
        cin >> konfirmasi;
        if (konfirmasi != 'y' && konfirmasi != 'Y') return root;
    }
    pushUndo("hapus", node->data);
    hapusDariPlaylist(judul);
    node->data.dihapus = true; // tandai sebagai dihapus tanpa menghapus dari memori
    return root;
}

Node* undoTerakhir(Node* root) {
    if (undoKosong()) {
        cout << "Tidak ada aksi untuk di-undo.\n";
        return root;
    }
    AksiUndo aksi = popUndo();

    if (aksi.tipe == "tambah") {
        cout << "Undo tambah video: " << aksi.data.judul << endl;
        root = hapusVideo(root, aksi.data.judul);
    } else if (aksi.tipe == "hapus") {
        cout << "Undo hapus video: " << aksi.data.judul << endl;
        Node* node = cari(root, aksi.data.judul);
        if (node) node->data.dihapus = false;
        else root = insert(root, aksi.data);
    } else if (aksi.tipe == "playlist") {
        cout << "Undo tambah ke playlist: " << aksi.data.judul << endl;
        hapusDariPlaylist(aksi.data.judul);
        Node* node = cari(root, aksi.data.judul);
        if (node) node->data.status = 0;
    } else if (aksi.tipe == "tonton") {
        cout << "Undo tonton video: " << aksi.data.judul << endl;
        Node* node = cari(root, aksi.data.judul);
        if (node) {
            node->data.status = 2;
            enqueue(&node->data);
        }
        if (atasRiwayat && atasRiwayat->data->judul == aksi.data.judul) {
            pop(atasRiwayat);
        }
    }
    return root;
}

int main() {
    Node* root = nullptr;
    int pilih;
    do {cout << "======================================\n";
        cout << "=========  MENU IDLIX TUBE   =========\n";
        cout << "======================================\n";
        cout << "1. Tambah Video\n";
        cout << "2. Tampilkan Daftar Video\n";
        cout << "3. Tambah ke Playlist\n";
        cout << "4. Tonton Video\n";
        cout << "5. Riwayat Tontonan\n";
        cout << "6. Hapus Video\n";
        cout << "7. Undo Tindakan Terakhir\n";
        cout << "8. Keluar\n";
        cout << "Pilih: ";
        cin >> pilih;
        cin.ignore();
        if (pilih == 1) {
            Video v;
            cout << "Masukkan Judul: "; 
                getline(cin, v.judul);
            cout << "Masukkan Durasi (menit): "; 
                cin >> v.durasi;
                cin.ignore();
            v.status = 0;
            root = insert(root, v);
            pushUndo("tambah", v);
        } else if (pilih == 2) {
            inorder(root);
            char cari;
            cout << "Apakah Anda ingin mencari video (y/n)? "; 
                cin >> cari; cin.ignore();
            if (cari == 'y' || cari == 'Y') {
                string j;
                cout << "Masukkan judul: "; getline(cin, j);
                cariVideo(root, j);
            }
        } else if (pilih == 3) {
            string j;
            cout << "Judul video yang ingin dimasukkan ke playlist: ";
            getline(cin, j);
            tambahKePlaylist(root, j);
        } else if (pilih == 4) {
            tontonVideo();
        } else if (pilih == 5) {
            cout << "\n=== Riwayat Tontonan ===\n";
            tampilkanRiwayat(atasRiwayat);
        } else if (pilih == 6) {
            string j;
            cout << "Judul video yang ingin dihapus: ";
            getline(cin, j);
            root = hapusVideo(root, j);
        } else if (pilih == 7) {
            root = undoTerakhir(root);
        } else if (pilih == 8) {
            exit(0);
        } else {
            cout << "pilihan tidak valid\n";
        }
    } while (pilih != 8);

    return 0;
}