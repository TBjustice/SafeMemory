// SafeMemory
// Created on April 29, 2023 by TrueBlueFeather
// MIT License

// SafeMemoryではメモリの先頭にヘッダー情報を追加して管理します。

#ifndef SAFEMEMORY
#define SAFEMEMORY

#ifdef _DEBUG
// デバッグ時のみログを表示
#include <stdio.h>
#endif

#include <stdlib.h>

// SafeMemoryを確保時、メモリの先頭に追加するヘッダー情報。
typedef struct {
	void* before; // 直前に確保されたSafeMemoryへのポインタ
	void* after; // 直後に確保されたSafeMemoryへのポインタ
} SafeMemoryHeader;

// SafeMemoryの先頭。ここからLinkedListのように、メモリをつなげていく。
SafeMemoryHeader safememorystart;

// SafeMemoryの終点。新しくメモリを確保する際、safememoryend->afterにその番地を代入する。
SafeMemoryHeader* safememoryend = &safememorystart;

// safememorystartの初期化。ぶっちゃけ要らないかも。
void InitSafeMemory() {
	safememorystart.before = NULL;
	safememorystart.after = NULL;
}

// 確保された全メモリを解放する。
void UninitSafememory() {
	//メモリの開放が先頭まで来たら解放終了
	while (safememoryend != &safememorystart) {
		// 今から解放するメモリの直前に確保したメモリを記憶しておく
		void* buf2 = (SafeMemoryHeader*)safememoryend->before;

#ifdef _DEBUG
		printf("Free[%llx]\n", (unsigned long long)safememoryend);
#endif

		// 解放
		free(safememoryend);

		// safememoryendを直前に確保したメモリにする
		safememoryend = (SafeMemoryHeader*)buf2;
	}
}

// 新たにメモリを確保する
void* SafeMemoryAllocate(size_t size) {
	void* buf1 = malloc(size + sizeof(SafeMemoryHeader)); // 確保したいメモリサイズ+ヘッダサイズを確保
	void* buf2 = (void*)safememoryend;

	// メモリの開放に失敗したら過去に確保したメモリを全解放してプログラム終了
	if (buf1 == NULL) {
#ifdef _DEBUG
		printf("SafeMemoryAllocate Fail\n");
#endif
		UninitSafememory();
		exit(1);
		return NULL; // 多分要らない
	}

	safememoryend->after = buf1; // (直前に確保したメモリ)->afterに今のアドレスを登録
	safememoryend = (SafeMemoryHeader*)buf1; // (直前に確保したメモリ)を今のアドレスにする。
	safememoryend->before = buf2; // (今のアドレス)->beforeに直前に確保したメモリをセットする
	safememoryend->after = NULL;

#ifdef _DEBUG
	printf("Allocate[%llx]\n", (unsigned long long)safememoryend);
#endif

	return (void*)(safememoryend + 1); // 確保したメモリのヘッダ以降のアドレスをリターンする
}

// あるメモリを解放する
void SafeMemoryRelease(void* data) {
	// ヘッダへのアドレスにするため、-1する。
	SafeMemoryHeader* buf = (SafeMemoryHeader*)data - 1;

	// もしも今から解放するアドレスにafterが存在しない(今のアドレスがsafememoryend)なら
	if (buf->after == NULL) {
		safememoryend = (SafeMemoryHeader*)buf->before; // safememoryendを(今のアドレス)->beforeにする。
	}
	else {
		// LinkedListの中身をくりぬくのだから以下の二つの値を書き換える
		// buf->after->before=buf->before
		// buf->before->after=buf->after
		((SafeMemoryHeader*)buf->after)->before = (SafeMemoryHeader*)buf->before;
		((SafeMemoryHeader*)buf->before)->after = (SafeMemoryHeader*)buf->after;
	}
	free(buf);

#ifdef _DEBUG
	printf("Free[%llx]\n", (unsigned long long)buf);
#endif
}
#endif
