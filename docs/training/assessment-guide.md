# Training assessment and lab record template

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Use this record for each training module in `README.md`. Training results are educational evidence only, not approval to ship a driver.

## Learner record
- Learner / reviewer:
- Date and source revision:
- Module / target platform / toolchain:
- VM or board identity and snapshot/rollback method:

## Lab evidence
- Requirements and expected result:
- Commands/build configuration:
- Test output/log location:
- Negative/fault cases exercised:
- Deviations and unresolved failures:
- Device/firmware/OS/SDK version:
- Data safety and cleanup confirmation:

## Review questions
1. Which buffers are user-controlled, and what bounds protect each access?
2. What owns an asynchronous request and its buffers before and after completion?
3. What happens when cancel, timeout, reset, removal, or power transition races with a request?
4. Which behavior is truly shared protocol logic, and which belongs to the platform/bus binding?
5. What does the simulator not model?
6. What evidence would be required before this result could support a production or safety-related release?

## Completion criteria
The learner can explain the design, reproduce the test, interpret failures, state limitations, and pass a peer review. Any unresolved memory-safety, data-loss, privilege, or safe-state issue blocks module completion.
