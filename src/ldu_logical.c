
bool ldu_logical_insert(struct object_struct *obj,
		void *head) {
	// Phase 1 : update-side removing logs
	if (!SWAP(&obj->ldu.remove.mark, false)){
		ASSERT(obj->ldu.insert.mark);
		obj->ldu.insert.mark = true;
		// Phase 2 : reuse garbage log
		if (!TEST_AND_SET_BIT(LDU_INSERT,
				&obj->ldu.used)){
			// Phase 3(slow-path): insert log to queue
			// ... : save argument and operation
			ldu_insert_queue(root, insert);
		}
	}
}

bool ldu_logical_remove(struct object_struct *obj,
		void *head) {
	// Phase 1 : update-side removing logs
	if (!SWAP(&obj->ldu.insert.mark, false)){
		ASSERT(obj->ldu.remove.mark);
		obj->ldu.remove.mark = true;
		// Phase 2 : reuse garbage log
		if (!TEST_AND_SET_BIT(LDU_REMOVE,
				&obj->ldu.used)){
			// Phase 3(slow-path): insert log to queue
			//... : save argument and operation
			ldu_insert_queue(root, insert);
		}
	}
}
