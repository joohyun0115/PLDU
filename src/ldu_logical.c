
bool ldu_logical_insert(struct vm_area_struct *vma,
		struct address_space *mapping)
{
	struct ldu_node *add_dnode = &vma->dnode.node[0];
	struct ldu_node *del_dnode = &vma->dnode.node[1];

	if (atomic_cmpxchg(&del_dnode->mark, 1, 0) != 1) {
		atomic_set(&add_dnode->mark, 1);
		if (!test_and_set_bit(LDU_OP_ADD, &vma->dnode.used)) {
			add_dnode->op_num = LDU_OP_ADD;
			add_dnode->key = vma;
			add_dnode->root = &mapping->i_mmap;
			i_mmap_ldu_logical_update(mapping, add_dnode);
		}
	}

	return true;
}

bool ldu_logical_remove(struct vm_area_struct *vma,
		struct address_space *mapping)
{
	struct ldu_node *add_dnode = &vma->dnode.node[0];
	struct ldu_node *del_dnode = &vma->dnode.node[1];

	if (atomic_cmpxchg(&add_dnode->mark, 1, 0) != 1) {
		atomic_set(&del_dnode->mark, 1);
		if (!test_and_set_bit(LDU_OP_DEL, &vma->dnode.used)) {
			del_dnode->op_num = LDU_OP_DEL;
			del_dnode->key = vma;
			del_dnode->root = &mapping->i_mmap;
			i_mmap_ldu_logical_update(mapping, del_dnode);
		}
	}

	return true;
}
