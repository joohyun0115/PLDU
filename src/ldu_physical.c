
void ldu_physical_update(int op,
		struct vm_area_struct *vma,
		struct rb_root *root)
{
	if (op == LDU_OP_ADD)
		vma_interval_tree_insert(vma, root);
	else
		vma_interval_tree_remove(vma, root);
}

void synchronize_ldu(struct address_space *mapping)
{
	struct llist_node *entry;
	struct ldu_node *dnode, *next;
	struct ldu_head *lduh = &mapping->lduh;

	entry = llist_del_all(&lduh->ll_head);
	llist_for_each_entry_safe(dnode,
			next, entry, ll_node) {
		struct vm_area_struct *vma =
				ACCESS_ONCE(dnode->key);
		if (atomic_cmpxchg(&dnode->mark,
				1, 0) == 1) {
			ldu_physical_update(dnode->op_num,
					vma,
					ACCESS_ONCE(dnode->root));
		}
		clear_bit(dnode->op_num, &vma->dnode.used);
		if (atomic_cmpxchg(&dnode->mark, 1, 0) == 1) {
			ldu_physical_update(dnode->op_num, vma,
					ACCESS_ONCE(dnode->root));
		}
	}
}

void free_work_func(struct work_struct *work)
{
	struct address_space *mapping =
			container_of(work, struct address_space,
			lduh.sync.work);
	struct llist_node *entry;
	struct vm_area_struct *vnode, *vnext;

	i_mmap_lock_write(mapping);
	synchronize_ldu_i_mmap(mapping);
	i_mmap_unlock_write(mapping);

	entry = llist_del_all(&mapping->llclean);
	llist_for_each_entry_safe(vnode,
			vnext, entry, llist) {
		if (!vnode->dnode.used)
			kmem_cache_free(vm_area_cachep,
					vnode);
		else
			llist_add(&vnode->llist,
					&mapping->llclean);
	}
}
