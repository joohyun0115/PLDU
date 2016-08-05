
bool gldu_logical_insert(struct vm_area_struct *vma,
  struct address_space *mapping)
{
	struct ldu_node *add = &vma->ldu.node[0];
	struct ldu_node *del = &vma->ldu.node[1];

	if(!xchg(&del->mark, 0)){
		add->mark = 1;
		if(!test_and_set_bit(LDU_ADD, &vma->ldu.used)){
			add->op_num = LDU_OP_ADD;
			add->key = vma;
			add->root = &mapping->i_mmap;
			ldu_logical_update(mapping, add);
		}
	}

	return true;
}

bool gldu_logical_remove(struct vm_area_struct *vma,
		struct address_space *mapping)
{
	struct ldu_node *add_dnode = &vma->dnode.node[0];
	struct ldu_node *del_dnode = &vma->dnode.node[1];

	if(atomic_cmpxchg(&add_dnode->mark, 1, 0) != 1) {
		atomic_set(&del_dnode->mark, 1);
		if(!test_and_set_bit(LDU_DEL, &vma->ldu.used)){
			del_dnode->op_num = LDU_OP_DEL;
			del_dnode->key = vma;
			del_dnode->root = &mapping->i_mmap;
			i_mmap_ldu_logical_update(mapping, del);
		}
	}

	return true;
}
