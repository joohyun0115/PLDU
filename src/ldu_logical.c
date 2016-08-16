
bool ldu_logical_insert(struct object_struct *obj,
		void *head)
{
	...
	// Phase 1 : update-side absorbing logs
	// atomic swap due to synchronize update's logs
	if(!xchg(&del->mark, 0)){
		BUG_ON(add->mark);
		// Phase 2 : reuse garbage log
		add->mark = 1;
		if(!test_and_set_bit(LDU_ADD,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
			ldu_insert_queue(root, add);
		}
	}

	return true;
}

bool ldu_logical_remove(struct object_struct *obj,
		void *head)
{
	...

	// Phase 1 : update-side absorbing logs
	// atomic swap due to synchronize update's logs
	if(!xchg(&add->mark, 0)){
		BUG_ON(del->mark);
		// Phase 2 : reuse garbage log
		del->mark = 1;
		if(!test_and_set_bit(LDU_DEL,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
		}
	}

	return true;
}
