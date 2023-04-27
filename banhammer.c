#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "ll.h"
#include "bv.h"
#include "bf.h"
#include "ht.h"
#include "parser.h"
#include "messages.h"

#define OPTIONS "ht:f:ms"

static void print_h(void) {
    fprintf(stderr, "Usage: ./banhammer [options]\n  ./banhammer will read in words from stdin, identify any badspeak or old speak and output an\n  appropriate punishment message. The badspeak and oldspeak (with the newspeak translation)\n  that caused the punishment will be printed after the message. If statistics are enabled\n  punishment messages are suppressed and only statistics will be printed.\n    -t <ht_size>: Hash table size set to <ht_size>. (default: 10000)\n    -f <bf_size>: Bloom filter size set to <bf_size>. (default 2^19)\n    -s          : Enables the printing of statistics.\n    -m          : Enables move-to-front rule.\n    -h          : Display program synopsis and usage.\n");
    return;
}

static void empty_s(char *s) {
    for (uint32_t i = 0; s[i]; i += 1) {
        s[i] = '\0';
    }
    return;
}

static void trim_char(char *s) {
    int i;
    for (i = 0; s[i]; i += 1);
    s[i-1] = '\0';
    return;
}

static void nextw(char *res, char *string, int *i) {
    int j = 0;
    while (string[*i] != '\0' && string[*i] != ' ') {
        res[j] = string[*i];
        *i += 1;
        j += 1;
    }
    res[j] = '\0';
    *i += 1;
    return;
}

static bool my_scmp_equal(char *w1, char *w2) {     //functions the same, maybe better than the my_strcmp in ll.c
    int i;
    for (i = 0; w1[i] && w2[i]; i += 1) {
        if (w1[i] != w2[i]) {
            return false;
        }
    }
    return true;
}

static int get_strlen(char *s) {
    int i;
    for (i = 0; s[i]; i += 1);
    return i;
}

int main(int argc, char **argv) {
    int opt = 0, s = 0;
    bool mtf = false;
    uint32_t ht_size = 10000, bf_siz = 0x80000;
    while ((opt = getopt(argc,argv,OPTIONS)) != -1) {
        switch(opt) {
            case 'h':
                print_h();
                exit(EXIT_SUCCESS);
                break;
            case 't':
                ht_size = strtoul(optarg, NULL, 10);
                break;
            case 'f':
                bf_siz = strtoul(optarg, NULL, 10);
                break;
            case 'm':
                mtf = true;
                break;
            case 's':
                s = 1;
                break;
            default:
                print_h();
                exit(EXIT_FAILURE);
                break;
        }
    }
    BloomFilter *bf = bf_create(bf_siz);                                    //initialized BloomFilter
    HashTable *ht = ht_create(ht_size, mtf);                                //initialized HashTable

    //input badspeak into BloomFilter and HashTable
    FILE *badspeak = fopen("badspeak.txt", "r");                            //initialized badspeak FILE
    char buffer[MAX_PARSER_LINE_LENGTH+1];
    while (fgets(buffer, MAX_PARSER_LINE_LENGTH+1, badspeak)) {
        trim_char(buffer);
        bf_insert(bf, buffer);
        ht_insert(ht, buffer, "NulL string");
        empty_s(buffer);
    }
    fclose(badspeak);


    //input newspeak into BloomFilter and HashTable
    FILE *newspeak = fopen("newspeak.txt", "r");                            //initialized newspeak FILE
    int i = 0;                                                              //i used to find old and new
    char old[MAX_PARSER_LINE_LENGTH+1], new[MAX_PARSER_LINE_LENGTH+1];
    while (fgets(buffer, MAX_PARSER_LINE_LENGTH+1, newspeak)) {
        trim_char(buffer);
        nextw(old, buffer, &i);
        nextw(new, buffer, &i);
        bf_insert(bf, old);
        ht_insert(ht, old, new);
        empty_s(old);
        empty_s(new);
        empty_s(buffer);
        i = 0;
    }
    fclose(newspeak);
    
    //read words from stdin using parser
    Parser *p;// = parser_create(stdin);                                       //initialized Parser
    LinkedList *thoughtcrime_corrections = ll_create(mtf);
    LinkedList *rightspeak_corrections = ll_create(mtf);
    char *word = (char *)malloc((MAX_PARSER_LINE_LENGTH+1)*sizeof(char));      //declared allocated word string
    while ((p = parser_create(stdin))) {
        while (next_word(p, word)) {
            if (get_strlen(word) == 0) {
                continue;
            }
            if (bf_probe(bf, word)) {
                Node *winh = ht_lookup(ht, word);
                if (winh && my_scmp_equal(winh->newspeak, "NulL string") && !ll_lookup(thoughtcrime_corrections, winh->oldspeak)) {     //needs string comparator to find if newspeak is "NULL"
                    ll_insert(thoughtcrime_corrections, winh->oldspeak, winh->newspeak);
                } else if (winh && !my_scmp_equal(winh->newspeak, "NulL string") && !ll_lookup(rightspeak_corrections, winh->oldspeak)) {
                    ll_insert(rightspeak_corrections, winh->oldspeak, winh->newspeak);
                }
            }
        }
        parser_delete(&p);
    }
    free(word);

    if (s == 0) {
        //print correct crime
        if (ll_length(thoughtcrime_corrections) > 0 && ll_length(rightspeak_corrections) > 0) {
            fprintf(stdout, "%s", mixspeak_message);
        } else if (ll_length(thoughtcrime_corrections) > 0) {
            fprintf(stdout, "%s", badspeak_message);
        } else if (ll_length(rightspeak_corrections) > 0) {
            printf("%s", goodspeak_message);
        }

        //print all corrections
        ll_print(thoughtcrime_corrections);
        ll_print(rightspeak_corrections);
    } else {
        //print stats
        uint32_t hnk, hnh, hnm, hne, bnk, bnh, bnm, bne;
        double bfl, ave_sl, fp, bits_epm;
        bfl = bf_count(bf) / bf_size(bf);
        ht_stats(ht, &hnk, &hnh, &hnm, &hne);                                           //ht stats
        bf_stats(bf, &bnk, &bnh, &bnm, &bne);                                           //bf stats
        if (bnm == 0) {
            bits_epm = 0;
        } else {
            bits_epm = ((float)bne - ((float)N_HASHES*bnh))/bnm;                      //Bits examined per miss
        }
        if (bnh == 0) { 
            fp = 0;
        } else {
            fp = (double)hnm / bnh;                                                     //False positives
        }
        if (bf_size(bf) == 0) {
            bfl = 0;
        } else {
            bfl = (double)bf_count(bf) / bf_size(bf);                                   //Bloom filter load
        }
        if ((hnh + hnm) == 0) {
            ave_sl = 0;
        } else {
            ave_sl = (double)hne / (hnh + hnm);                                         //Average seek length
        }
        fprintf(stdout, "ht keys: %u\nht hits: %u\nht misses: %u\nht probes: %u\nbf keys: %u\nbf hits: %u\nbf misses: %u\nbf bits examined: %u\nBits examined per miss: %f\nFalse positives: %f\nAverage seek length: %f\nBloom filter load: %f\n", hnk, hnh, hnm, hne, bnk, bnh, bnm, bne, bits_epm, fp, ave_sl, bfl);
    }

    //delete allocated structures
    ll_delete(&thoughtcrime_corrections);
    ll_delete(&rightspeak_corrections);
    ht_delete(&ht);
    bf_delete(&bf);
    return 0;
}
