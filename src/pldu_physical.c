
void ldu_physical_update(int op,
		struct vm_area_struct *vma,
		struct rb_root *root)
{
	if (!root)
		return;

	if (op == LDU_OP_ADD)
		vma_interval_tree_insert(vma, root);
	else
		vma_interval_tree_remove(vma, root);
}

void synchronize_ldu(struct llist_node *entry)
{
	struct ldu_node *dnode;
	struct address_space *mapping;
	struct vm_area_struct *vma;

	llist_for_each_entry(dnode,
			entry, ll_node) {
		vma = READ_ONCE(dnode->key);
		if (atomic_cmpxchg(
				&dnode->mark, 1, 0) == 1) {
			ldu_physical_update(
					dnode->op_num, vma,
					READ_ONCE(dnode->root));

		}
		clear_bit(dnode->op_num,
				&vma->dnode.used);
		if (atomic_cmpxchg(
				&dnode->mark, 1, 0) == 1) {
			ldu_physical_update(
					dnode->op_num, vma,
					READ_ONCE(dnode->root));
		}
	}
}
