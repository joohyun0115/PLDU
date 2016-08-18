
bool ldu_logical_insert(struct object_struct *obj,
		void *head)
{
	...
	// Phase 1 : update-side absorbing logs
	// atomic swap due to synchronize update's logs
	if(!xchg(&remove->mark, 0)){
		BUG_ON(insert->mark);
		// Phase 2 : reuse garbage log
		insert->mark = 1;
		if(!test_and_set_bit(LDU_INSERT,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
			ldu_insert_queue(root, insert);
		}
	}
	...
}

bool ldu_logical_remove(struct object_struct *obj,
		void *head)
{
	...

	// Phase 1 : update-side absorbing logs
	// atomic swap due to synchronize update's logs
	if(!xchg(&insert->mark, 0)){
		BUG_ON(remove->mark);
		// Phase 2 : reuse garbage log
		remove->mark = 1;
		if(!test_and_set_bit(LDU_REMOVE,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
		}
	}
	...
}
