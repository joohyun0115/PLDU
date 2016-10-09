
bool ldu_logical_insert(struct object_struct *obj,
		void *head) {
	// Phase 1 : update-side removing logs
	// atomic swap due to synchronize function
	if (!xchg(&obj->ldu.remove.mark, false)){
		BUG_ON(obj->ldu.insert.mark);
		obj->ldu.insert.mark = true;
		// Phase 2 : reuse garbage log
		if (!test_and_set_bit(LDU_INSERT,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
			ldu_insert_queue(root, insert);
		}
	}
}

bool ldu_logical_remove(struct object_struct *obj,
		void *head) {
	// Phase 1 : update-side removing logs
	// atomic swap due to synchronize function
	if (!xchg(&obj->ldu.insert.mark, false)){
		BUG_ON(obj->ldu.remove.mark);
		obj->ldu.remove.mark = true;
		// Phase 2 : reuse garbage log
		if (!test_and_set_bit(LDU_REMOVE,
				&obj->ldu.used)){
			// Phase 3 : insert log to queue
			//...save argument and operation
			ldu_insert_queue(root, insert);
		}
	}
}
