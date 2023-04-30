// SafeMemory
// Created on April 29, 2023 by TrueBlueFeather
// MIT License

// SafeMemory�ł̓������̐擪�Ƀw�b�_�[����ǉ����ĊǗ����܂��B

#ifndef SAFEMEMORY
#define SAFEMEMORY

#ifdef _DEBUG
// �f�o�b�O���̂݃��O��\��
#include <stdio.h>
#endif

#include <stdlib.h>

// SafeMemory���m�ێ��A�������̐擪�ɒǉ�����w�b�_�[���B
typedef struct {
	void* before; // ���O�Ɋm�ۂ��ꂽSafeMemory�ւ̃|�C���^
	void* after; // ����Ɋm�ۂ��ꂽSafeMemory�ւ̃|�C���^
} SafeMemoryHeader;

// SafeMemory�̐擪�B��������LinkedList�̂悤�ɁA���������Ȃ��Ă����B
SafeMemoryHeader safememorystart;

// SafeMemory�̏I�_�B�V�������������m�ۂ���ہAsafememoryend->after�ɂ��̔Ԓn��������B
SafeMemoryHeader* safememoryend = &safememorystart;

// safememorystart�̏������B�Ԃ����Ⴏ�v��Ȃ������B
void InitSafeMemory() {
	safememorystart.before = NULL;
	safememorystart.after = NULL;
}

// �m�ۂ��ꂽ�S���������������B
void UninitSafememory() {
	//�������̊J�����擪�܂ŗ��������I��
	while (safememoryend != &safememorystart) {
		// �����������郁�����̒��O�Ɋm�ۂ������������L�����Ă���
		void* buf2 = (SafeMemoryHeader*)safememoryend->before;

#ifdef _DEBUG
		printf("Free[%llx]\n", (unsigned long long)safememoryend);
#endif

		// ���
		free(safememoryend);

		// safememoryend�𒼑O�Ɋm�ۂ����������ɂ���
		safememoryend = (SafeMemoryHeader*)buf2;
	}
}

// �V���Ƀ��������m�ۂ���
void* SafeMemoryAllocate(size_t size) {
	void* buf1 = malloc(size + sizeof(SafeMemoryHeader)); // �m�ۂ������������T�C�Y+�w�b�_�T�C�Y���m��
	void* buf2 = (void*)safememoryend;

	// �������̊J���Ɏ��s������ߋ��Ɋm�ۂ�����������S������ăv���O�����I��
	if (buf1 == NULL) {
#ifdef _DEBUG
		printf("SafeMemoryAllocate Fail\n");
#endif
		UninitSafememory();
		exit(1);
		return NULL; // �����v��Ȃ�
	}

	safememoryend->after = buf1; // (���O�Ɋm�ۂ���������)->after�ɍ��̃A�h���X��o�^
	safememoryend = (SafeMemoryHeader*)buf1; // (���O�Ɋm�ۂ���������)�����̃A�h���X�ɂ���B
	safememoryend->before = buf2; // (���̃A�h���X)->before�ɒ��O�Ɋm�ۂ������������Z�b�g����
	safememoryend->after = NULL;

#ifdef _DEBUG
	printf("Allocate[%llx]\n", (unsigned long long)safememoryend);
#endif

	return (void*)(safememoryend + 1); // �m�ۂ����������̃w�b�_�ȍ~�̃A�h���X�����^�[������
}

// ���郁�������������
void SafeMemoryRelease(void* data) {
	// �w�b�_�ւ̃A�h���X�ɂ��邽�߁A-1����B
	SafeMemoryHeader* buf = (SafeMemoryHeader*)data - 1;

	// ������������������A�h���X��after�����݂��Ȃ�(���̃A�h���X��safememoryend)�Ȃ�
	if (buf->after == NULL) {
		safememoryend = (SafeMemoryHeader*)buf->before; // safememoryend��(���̃A�h���X)->before�ɂ���B
	}
	else {
		// LinkedList�̒��g������ʂ��̂�����ȉ��̓�̒l������������
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
