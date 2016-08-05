
bool pldu_logical_update(struct address_space *mapping,
		struct ldu_node *dnode)
{
	struct pldu_deferred_i_mmap *p;
	struct i_mmap_slot *slot;
	struct llist_node *first;
	struct llist_node *entry;
	struct ldu_node *ldu;

	slot = &get_cpu_var(i_mmap_slot);
	p = &slot->mapping[hash_ptr(mapping, HASH_ORDER)];
	first = READ_ONCE(p->list.first);
	if (first) {
		ldu = llist_entry(first, struct ldu, ll_node);
		if (ldu->root != dnode->root) {
			//pr_info("conflict hash table\n");
			locked_mapping = READ_ONCE(ldu->key2);
			entry = llist_del_all(&p->list);
			llist_add(&dnode->ll_node, &p->list);
			put_cpu_var(i_mmap_slot);
			down_write(&locked_mapping->i_mmap_rwsem);
			synchronize_ldu_i_mmap_internal(entry);
			up_write(&locked_mapping->i_mmap_rwsem);
			goto out;
		}
	}

	llist_add(&dnode->ll_node, &p->list);
	put_cpu_var(i_mmap_slot);

out:
	return true;
}

bool pldu_logical_insert(
		struct vm_area_struct *vma,
		struct address_space *mapping)
{
	struct ldu_node *add = &vma->dnode.node[0];
	struct ldu_node *del = &vma->dnode.node[1];

	if (atomic_cmpxchg(&del->mark, 1, 0) != 1) {
		BUG_ON(atomic_read(&add->mark));
		atomic_set(&add_dnode->mark, 1);
		if (!test_and_set_bit(OP_ADD, &vma->used)) {
			add->op_num = OP_ADD;
			add->key = vma;
			add->key2 = mapping;
			add->root = &mapping->i_mmap;
			ldu_logical_update(mapping, add_dnode);
		}
	}

	return true;
}
