#ifndef BPT_H
#define BPT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#ifndef UNIT_TEST
#include "predefined.h"
#else
#include "util/unit_test_predefined.h"
#endif

namespace bpt {

/* offsets */
#define OFFSET_META 0
#define OFFSET_BLOCK OFFSET_META + sizeof(meta_t)
#define SIZE_NO_CHILDREN sizeof(leaf_node_t) - BP_ORDER * sizeof(record_t)

/* meta information of B+ tree */
typedef struct {
    size_t order; /* `order` of B+ tree */
    size_t value_size; /* size of value */
    size_t key_size;   /* size of key */
    size_t internal_node_num; /* how many internal nodes */
    size_t leaf_node_num;     /* how many leafs */
    size_t height;            /* height of tree (exclude leafs) */
    off_t slot;        /* where to store new block */
    off_t root_offset; /* where is the root of internal nodes */
    off_t leaf_offset; /* where is the first leaf */

    /*---------------- MSRIT Researchers -----------------*/
    size_t multithreading_degree; /* number of threads supported */
    size_t number_of_threads; /* number of threads currently maintained */
    off_t thread_offsets[MULTITHREADING_DEGREE+1];
} meta_t;

/* Each <key, child> is wrapped in an index_t structure */
struct index_t {
    key_t key;
    off_t child; /* child's offset , to locate child*/
    // This is a data type defined in the sys/types.h header file (of fundamental type unsigned long) and is used to measure the file offset in bytes from the beginning of the file.
};

/***
 * internal node block
 ***/
struct internal_node_t {
    /* pointer to child*/
    typedef index_t * child_t;
    
    off_t parent; /* parent node offset */
    off_t next; /* ?? */
    off_t prev; /* ?? */
    size_t n; /* how many children */
    
    index_t children[BP_ORDER]; /* array of keys and offsets */
};

/* the final record */
struct record_t {
    key_t key;
    value_t value; /*TODO : Replace with our custom values, which represent each element from dataset*/
};

/* leaf node block */
struct leaf_node_t {
    typedef record_t *child_t;

    off_t parent; /* parent node offset */
    off_t next;
    off_t prev;
    size_t n;
    record_t children[BP_ORDER]; /* array of keys and records */
};

/* the encapulated B+ tree */
class bplus_tree {
public:
    /*---------------- MSRIT Researchers -----------------*/
    // off_t thread_offsets[MULTITHREADING_DEGREE]; /* offset that each thread must begin at */

    bplus_tree(const char *path, bool force_empty = false);

    /* abstract operations */
    int search(const key_t& key, value_t *value) const;
    int search_range(key_t *left, const key_t &right,
                     value_t *values, size_t max, bool *next = NULL) const;
    int remove(const key_t& key);
    int insert(const key_t& key, value_t value,bool lastValue=false);
    int update(const key_t& key, value_t value);
    
    template<class T>
    int run_map(T* block, off_t offset){

        size_t size = sizeof(T);
        open_file();
        int num = fseek(fp, offset, SEEK_SET);
        if(num<0){
            std::cout<<"error"<<std::endl;
            return -1;
        }

        size_t rd = fread(block, size, 1, fp);
        close_file();

        return rd - 1;
    }

    meta_t get_meta() const {
        return meta;
    };

#ifndef UNIT_TEST
private:
#else
public:
#endif
    char PATH[512];
    meta_t meta;

    void compute_thread_offsets(off_t node_offset, int child_number, int number_of_threads );

    void compute_thread_offsets_max();
    void compute_thread_offsets_level_order(internal_node_t root,int value);

    /* init empty tree */
    void init_from_empty();

    /* find index */
    off_t search_index(const key_t &key) const;

    /* find leaf */
    off_t search_leaf(off_t index, const key_t &key) const;
    off_t search_leaf(const key_t &key) const
    {
        return search_leaf(search_index(key), key);
    }

    /* remove internal node */
    void remove_from_index(off_t offset, internal_node_t &node,
                           const key_t &key);

    /* borrow one key from other internal node */
    bool borrow_key(bool from_right, internal_node_t &borrower,
                    off_t offset);

    /* borrow one record from other leaf */
    bool borrow_key(bool from_right, leaf_node_t &borrower);

    /* change one's parent key to another key */
    void change_parent_child(off_t parent, const key_t &o, const key_t &n);

    /* merge right leaf to left leaf */
    void merge_leafs(leaf_node_t *left, leaf_node_t *right);

    void merge_keys(index_t *where, internal_node_t &left,
                    internal_node_t &right, bool change_where_key = false);

    /* insert into leaf without split */
    void insert_record_no_split(leaf_node_t *leaf,
                                const key_t &key, const value_t &value);

    /* add key to the internal node */
    void insert_key_to_index(off_t offset, const key_t &key,
                             off_t value, off_t after);
    void insert_key_to_index_no_split(internal_node_t &node, const key_t &key,
                                      off_t value);

    /* change children's parent */
    void reset_index_children_parent(index_t *begin, index_t *end,
                                     off_t parent);

    //------------MSRIT researchers-----------------
    //void compute_thread_pointers(off_t node_offset, int number_of_threads=1);
    //----------------------------------------------

    /* Template Class: a class that allows the programmer to operate with generic data types */
    template<class T>
    void node_create(off_t offset, T *node, T *next);

    template<class T>
    void node_remove(T *prev, T *node);

    /* multi-level file open/close */
    mutable FILE *fp;
    /* count of how many files are opened*/
    mutable int fp_level;
    void open_file(const char *mode = "rb+") const
    {
        // `rb+` will make sure we can write everywhere without truncating file
        if (fp_level == 0)
            fp = fopen(PATH, mode);

        ++fp_level;
    }

    void close_file() const
    {
        if (fp_level == 1)
            fclose(fp);

        --fp_level;
    }

    /* alloc from disk */
    off_t alloc(size_t size)
    {
        off_t slot = meta.slot;
        meta.slot += size;
        return slot;
    }

    off_t alloc(leaf_node_t *leaf)
    {
        leaf->n = 0;
        meta.leaf_node_num++;
        return alloc(sizeof(leaf_node_t));
    }

    off_t alloc(internal_node_t *node)
    {
        node->n = 1;
        meta.internal_node_num++;
        return alloc(sizeof(internal_node_t));
    }

    void unalloc(leaf_node_t *leaf, off_t offset)
    {
        --meta.leaf_node_num;
    }

    void unalloc(internal_node_t *node, off_t offset)
    {
        --meta.internal_node_num;
    }

    /* read block from disk */
    int map(void *block, off_t offset, size_t size) const
    {
        open_file();
        fseek(fp, offset, SEEK_SET);
        size_t rd = fread(block, size, 1, fp);
        close_file();

        return rd - 1;
    }

    template<class T>
    int map(T *block, off_t offset) const
    {
        return map(block, offset, sizeof(T));
    }

    /* write block to disk */
    int unmap(void *block, off_t offset, size_t size) const
    {
        open_file();
        fseek(fp, offset, SEEK_SET);
        size_t wd = fwrite(block, size, 1, fp);
        close_file();

        return wd - 1;
    }

    template<class T>
    int unmap(T *block, off_t offset) const
    {
        return unmap(block, offset, sizeof(T));
    }
};

}

#endif /* end of BPT_H */
