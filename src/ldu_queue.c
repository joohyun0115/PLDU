
bool insert_log_global_queue(struct obj_root *root,
		struct ldu_node *dnode)
{
	...
	//do {
	//	new_last->next = first = head->first;
	//} while (cmpxchg(&head->first, first, new_first) != first);
	llist_add(&dnode->ll_node, &p->list);
	...
}

bool insert_log_per_core_queue(struct obj_root *root,
		struct ldu_node *dnode)
{
	...

	slot = &get_cpu_var(obj_root_slot);
	p = &slot->obj[hash_ptr(root, HASH_ORDER)];
	empty = READ_ONCE(p->list.first);
	// is empty list?
	if (!empty) {
		ldu = llist_entry(first, struct ldu, ll_node);
		// is hash complict?
		if (ldu->root != dnode->root) {
			...
			lock = READ_ONCE(ldu->lock);
			entry = xchg(&p->list->head->first, NULL);
			llist_add(&dnode->ll_node, &p->list);
			// flush log as a direct mapped cache
			object_lock(&lock);
			synchronize_ldu(entry);
			object_unlock(&lock);
			goto out;
		}
	}

	llist_add(&dnode->ll_node, &p->list);
out:
	...
}
