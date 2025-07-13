#include "rift-0/core/rift_compat.h"
#include <stdio.h>

/* RIFT_CLEANUP: Forward declarations to resolve type issues */
#ifndef RIFT_CLEANUP_FORWARD_DECLS
#define RIFT_CLEANUP_FORWARD_DECLS

/* Forward declare missing types if not already defined */
#ifndef RIFT_TOKEN_TYPE_DEFINED
#define RIFT_TOKEN_TYPE_DEFINED
typedef enum TokenType RiftTokenType;
#endif

/* Resolve HeapQueue issues in lexer_flag.h */
#ifndef HEAPQUEUE_FORWARD_DECLARED
#define HEAPQUEUE_FORWARD_DECLARED
struct HeapQueue;
typedef struct HeapQueue HeapQueue;

typedef struct {
    int priority;
    int value;
} Item;
#endif

/* Add strdup declaration if missing */
#ifndef HAVE_STRDUP
#ifdef __cplusplus
extern "C" {
#endif
char* strdup(const char* s);
#ifdef __cplusplus
}
#endif
#endif

#endif /* RIFT_CLEANUP_FORWARD_DECLS */



/* RIFT_CLEANUP: Forward declarations to resolve type issues */
#ifndef RIFT_CLEANUP_FORWARD_DECLS
#define RIFT_CLEANUP_FORWARD_DECLS

/* Forward declare missing types if not already defined */
#ifndef RIFT_TOKEN_TYPE_DEFINED
#define RIFT_TOKEN_TYPE_DEFINED
typedef enum TokenType RiftTokenType;
#endif

/* Resolve HeapQueue issues in lexer_flag.h */
#ifndef HEAPQUEUE_FORWARD_DECLARED
#define HEAPQUEUE_FORWARD_DECLARED
struct HeapQueue;
typedef struct HeapQueue HeapQueue;

typedef struct {
    int priority;
    int value;
} Item;
#endif

/* Add strdup declaration if missing */
#ifndef HAVE_STRDUP
#ifdef __cplusplus
extern "C" {
#endif
char* strdup(const char* s);
#ifdef __cplusplus
}
#endif
#endif

#endif /* RIFT_CLEANUP_FORWARD_DECLS */


#include <stdlib.h>
#include <string.h>
#include "rift-0/core/gov/rift-gov.0.h"

// Min-heap helpers for RiftStageQueue
static void heapify_up(RiftStageQueue* queue, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (queue->entries[idx].priority < queue->entries[parent].priority) {
            RiftStageEntry tmp = queue->entries[idx];
            queue->entries[idx] = queue->entries[parent];
            queue->entries[parent] = tmp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void heapify_down(RiftStageQueue* queue, size_t idx) {
    size_t left, right, smallest;
    while (1) {
        left = 2 * idx + 1;
        right = 2 * idx + 2;
        smallest = idx;
        if (left < queue->count && queue->entries[left].priority < queue->entries[smallest].priority)
            smallest = left;
        if (right < queue->count && queue->entries[right].priority < queue->entries[smallest].priority)
            smallest = right;
        if (smallest != idx) {
            RiftStageEntry tmp = queue->entries[idx];
            queue->entries[idx] = queue->entries[smallest];
            queue->entries[smallest] = tmp;
            idx = smallest;
        } else {
            break;
        }
    }
}

void rift_stage_queue_init(RiftStageQueue* queue, size_t capacity) {
    queue->entries = (RiftStageEntry*)calloc(capacity, sizeof(RiftStageEntry));
    queue->count = 0;
    queue->capacity = capacity;
}

void rift_stage_queue_free(RiftStageQueue* queue) {
    if (queue->entries) free(queue->entries);
    queue->entries = NULL;
    queue->count = 0;
    queue->capacity = 0;
}

void rift_stage_queue_push(RiftStageQueue* queue, const RiftStageEntry* entry) {
    if (queue->count >= queue->capacity) {
        queue->capacity *= 2;
        queue->entries = (RiftStageEntry*)realloc(queue->entries, queue->capacity * sizeof(RiftStageEntry));
    }
    queue->entries[queue->count] = *entry;
    heapify_up(queue, queue->count);
    queue->count++;
}

RiftStageEntry rift_stage_queue_pop(RiftStageQueue* queue) {
    RiftStageEntry result = {0};
    if (queue->count == 0) return result;
    result = queue->entries[0];
    queue->entries[0] = queue->entries[queue->count - 1];
    queue->count--;
    heapify_down(queue, 0);
    return result;
}

bool rift_stage_queue_empty(const RiftStageQueue* queue) {
    return queue->count == 0;
}

RiftStageEntry* rift_stage_queue_find_by_id(RiftStageQueue* queue, int stage_id) {
    for (size_t i = 0; i < queue->count; i++) {
        if (queue->entries[i].stage_id == stage_id) return &queue->entries[i];
    }
    return NULL;
}

RiftStageEntry* rift_stage_queue_find_by_name(RiftStageQueue* queue, const char* name) {
    for (size_t i = 0; i < queue->count; i++) {
        if (strcmp(queue->entries[i].name, name) == 0) return &queue->entries[i];
    }
    return NULL;
}

// Minimal XML/JSON loader stubs (expand as needed)
void rift_stage_tracker_load_from_xml(RiftStageQueue* queue, const char* xml_path) {
    // TODO: Parse XML and fill queue->entries
    // For now, add a single dummy entry for demonstration
    RiftStageEntry entry = {0};
    entry.stage_id = 0;
    entry.priority = 0;
    strcpy(entry.name, "tokenization");
    strcpy(entry.description, "Stage-0 Tokenization");
    entry.active = true;
    rift_stage_queue_push(queue, &entry);
}

void rift_stage_tracker_load_from_json(RiftStageQueue* queue, const char* json_path) {
    // TODO: Parse JSON and fill queue->entries
    // For now, add a single dummy entry for demonstration
    RiftStageEntry entry = {0};
    entry.stage_id = 0;
    entry.priority = 0;
    strcpy(entry.name, "tokenization");
    strcpy(entry.description, "Stage-0 Tokenization");
    entry.active = true;
    rift_stage_queue_push(queue, &entry);
}
